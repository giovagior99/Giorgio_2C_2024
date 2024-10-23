/*! @mainpage Guia2_Ej2
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |    ESP32   	|
 * |:--------------:|:--------------|
 * | 	  ECHO	 	| 	 GPIO_3		|
 * | 	 TRIGGER 	| 	 GPIO_2		|
 * |      +5V	 	| 	  +5V		|
 * | 	  GND	 	| 	  GND		|
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 18/09/2024 | Document creation		                         |
 * | 23/10/2024 | End obligatory part, UART, On and Hold                         |
 *
 * @author Giovanni Giorgio (giovanni.giorgio@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_mcu.h"
#include "hc_sr04.h"
#include "led.h"
#include "switch.h"
#include "lcditse0803.h"
#include "timer_mcu.h"
#include "uart_mcu.h"

/*==================[macros and definitions]=================================*/
/** @def CONFIG_MODE_PERIOD
 * @brief Periodo de tiempo de refresco de modo (teclas encendido o hold)
 */
#define CONFIG_MODE_PERIOD 100

/** @def CONFIG_MEASURE_PERIOD
 * @brief Periodo de tiempo de refresco de medición
 */
#define CONFIG_MEASURE_PERIOD 1000000 //en microsegundos

/** @def CONFIG_SHOW_PERIOD
 * @brief Periodo de tiempo de refresco de mostrar
 */
#define CONFIG_SHOW_PERIOD 500000 //en microsegundos

/*==================[internal data definition]===============================*/
/** @def tecON
 * @brief Booleano para encender o apagar la medición
 */
bool tecON = 0; //Por defecto esta apagado

/** @def tecHOLD
 * @brief Booleano para mantener la medicion en el display
*/
bool tecHOLD = 0; //Por defecto no mantiene la medición

/** @def distancia
 * @brief Distancia medida
 */
uint16_t distancia = 0;

/** @def medir_task_handle
 *  @brief
 */
TaskHandle_t medir_task_handle = NULL;

/** @def mostrar_task_handle
 * @brief
 */
TaskHandle_t mostrar_task_handle = NULL;

/** @def comunicar_task_handle
 * @brief
 */
TaskHandle_t comunicar_task_handle = NULL;

/** @def tecCmIn
 * @brief Tecla que configura la unidad de medida con la que se muetra la distancia
 */
bool tecCmIn = 0; // 0 en cm, 1 en in

/** @def tecMAX
 * @brief Tecla que muestra por pantantalla la maxima medicion
 */
bool tecMAX = 0;

/*==================[internal functions declaration]=========================*/
/** @def void OnOffSwitch(void *pvParameter)
 * @brief Funcion que cuando se presiona el Switch 1 cambia el estado de tecON para encender 
 * o apagar
 * @param [in] NULL
*/
void OnOffSwitch(void *pvParameter){
	tecON = !tecON;	
}

/** @def void OnOffSwitch(void *pvParameter)
 * @brief Funcion que cuando se presiona el Switch 2 cambia el estado de tecHOLD para mantener la medicion
 * @param [in] NULL
*/
void HoldSwitch(void *pvParameter){
	tecHOLD = !tecHOLD;	
}

/** @def static void MedirTask(void *pvParameter)
 * @brief Tarea encargada de medir la distancia cada cierto periodo de tiempo
 * @param[in] pvParameter void* que corresponde a los parametros de la tarea
 */
static void MedirTask(void *pvParameter){
    while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    /* La tarea espera en este punto hasta recibir una notificación */
		if (tecON) //Si tecON == 1 se mide
		{
			distancia = HcSr04ReadDistanceInCentimeters();
		}
    }
}

/** @def static void MostrarTask(void *pvParameter)
 * @brief Tarea encargada de mostrar la distancia en el display y leds dependiendo del estado de On y Hold.
 * Si tecON == 0 no se muestra nada
 * Si tecON == 1 se muestra la distancia
 * Si tecHOLD == 0 se actualiza la distancia mostrada
 * Si tecHOLD == 1 se mantiene lo mostrado
 * @param[in] pvParameter void* que corresponde a los métodos de la tarea
 */
static void MostrarTask(void *pvParameter){
	uint16_t dmostrar = 0; //Distancia a mostrar
	char buffer[12]; //Para mostrar la distancia por uart
    while(true)
	{	
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    /* La tarea espera en este punto hasta recibir una notificación */
		if(tecON) //Si tecON == 1 se muestra la distancia
		{
			if(!tecHOLD)
			{ 
				dmostrar = distancia; //Si tecHOLD == 0 se actualiza la distancia a mostrar
    		}

			LcdItsE0803Write(dmostrar);

			// sprintf(buffer, "%03d cm\r\n", dmostrar); //Escribo el estring a mostrar
            UartSendString(UART_PC, (char*)UartItoa(dmostrar, 10)); //Mando el string por UART
            UartSendString(UART_PC, " cm\r\n"); //Mando el string por UART

			LedsOffAll();

			if((dmostrar > 0) && (dmostrar <= 10))
			{
    			LedsOffAll();
    		}
    		if((dmostrar > 10) && (dmostrar <= 20))
			{
    			LedOn(LED_1);
    		}
    		if((dmostrar > 20) && (dmostrar <= 30)) 
			{
    			LedOn(LED_1);
    			LedOn(LED_2);
    		}
    		if(dmostrar > 30) {
    			LedOn(LED_1);
    			LedOn(LED_2);
    			LedOn(LED_3);
    		}
		}
		else
		{
			LedsOffAll();
			LcdItsE0803Off();
		}
    }
}

/** @def void FuncTimerA(void* param)
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada al LED_1 */
}

/**  @def void FuncTimerB(void* param)
 * @brief Función invocada en la interrupción del timer B
 */
void FuncTimerB(void* param){
    vTaskNotifyGiveFromISR(mostrar_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada al LED_2 */
}

/**  @def void FuncUart(void* param)
 * @brief Función invocada al recibir un dato por UART
 */
void FuncUart(void* param){
	uint8_t dato;
    UartReadByte(UART_PC, &dato);
	UartSendByte(UART_PC, (char *) &dato);

	switch(dato){
	case 'O':
		tecON = !tecON;
		break;
	case 'H':
		tecHOLD = !tecHOLD;
		break;
	case 'I':
		tecCmIn = !tecCmIn;
		break;
	case 'M':
		tecMAX = !tecMAX;
		break;
	default:
		break;
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){

	/* initializations */
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();
	LedsInit();

	timer_config_t timer_medicion = {
        .timer = TIMER_A,
        .period = CONFIG_MEASURE_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_medicion);

	timer_config_t timer_mostrar = {
        .timer = TIMER_B,
        .period = CONFIG_SHOW_PERIOD,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
    TimerInit(&timer_mostrar);

	serial_config_t puertoSerie={			
		.port =	UART_PC,
		.baud_rate = 9600,
		.func_p = FuncUart,
		.param_p = NULL
	};
	UartInit(&puertoSerie);

	/*tasks*/
	SwitchActivInt(SWITCH_1, OnOffSwitch, NULL);
	SwitchActivInt(SWITCH_2, HoldSwitch, NULL);
    xTaskCreate(&MedirTask, "Medir", 512, NULL, 5, &medir_task_handle);
	xTaskCreate(&MostrarTask, "Mostrar", 1024, NULL, 5, &mostrar_task_handle);

	/*timers start*/
    TimerStart(timer_medicion.timer);
    TimerStart(timer_mostrar.timer);
    
}
/*==================[end of file]============================================*/