/*! @mainpage examen_final
 *
 * @section genDesc General Description
 *
 * Examen final de la asignatura Electronica Programable 03/12/2024
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * |     LED 1	 	| 	 GPIO_11	|
 * | 	 ECHO	 	| 	 GPIO_3		|
 * | 	TRIGGER 	| 	 GPIO_2		|
 * |   VALVULA_AGU 	| 	 GPIO_23	|
 * |    ADC CH1		| 	 GPIO_1		|
 * |   VALVULA_ALI 	| 	 GPIO_2		|
 * |    UART TX 	| 	 GPIO_16	|
 * |    UART RX 	| 	 GPIO_17	|
 * |      +5V	 	| 	  +5V		|
 * | 	  GND	 	| 	  GND		|
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 03/12/2024 | Document creation		                         |
 *
 * @author Giovanni Giorgio (giovanni.giorgio@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include "hc_sr04.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

/** @def CONFIG_MEASURE_PERIOD
 * @brief Periodo de tiempo de refresco de medición de los recipientes y accionamiento
 * de las valvulas
 */
#define CONFIG_MEASURE_PERIOD 500000 //en microsegundos

/** @def CONFIG_SHOW_PERIOD
 * @brief Periodo de tiempo para comunicar militros de agua en el recipiente y gramos
 * de alimento
 */
#define CONFIG_COMUNICATION_PERIOD 500000 //en microsegundos

/*==================[internal data definition]===============================*/

/** @def tecON
 * @brief Booleano para encender o apagar el dispositivo
 */
bool tecON = 0; //Por defecto esta apagado

/** @def medirllenar_task_handle
 *  @brief -
 */
TaskHandle_t medirllenar_task_handle = NULL;

/** @def mostrar_task_handle
 * @brief -
 */
TaskHandle_t comunicar_task_handle = NULL;

/** @def distancia
 * @brief Distancia medida para calcular la cantidad de agua en el recipiente
 */
uint16_t distancia = 0;

/** @def mlagua
 * @brief Cantidad de agua en el recipiente en mililitros
 */
uint16_t mlagua = 0;

/** @def peso
 * @brief Peso de alimento medido en el recipiente
 */
uint16_t peso = 0;

/*==================[internal functions declaration]=========================*/

/** @def void FuncTimerA(void* param)
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerA(void* param){
	vTaskNotifyGiveFromISR(medirllenar_task_handle, pdFALSE); // Envía una notificación a la tarea asociada
}

/** @def void FuncTimerB(void* param)
 * @brief Función invocada en la interrupción del timer B
 */
void FuncTimerB(void* param){
	vTaskNotifyGiveFromISR(comunicar_task_handle, pdFALSE); // Envía una notificación a la tarea asociada
}

/** @def void OnOffSwitch(void *pvParameter)
 * @brief Funcion que cuando se presiona el SWITCH 1 cambia el estado de tecON para encender 
 * o apagar el dispositivo y prende o apagar el LED 1 correspondientemente
 * @param [in] NULL
*/
void OnOffSwitch(void *pvParameter){
	tecON = !tecON;

	if(tecON){
		LedOn(LED_1);
	}
	else{
		LedsOffAll();
	}
}

/** @def static void MedirLlenarTask(void *pvParameter)
 * @brief Tarea encargada de medir la altura del recipiente de agua y el peso recipiente de
 * alimento, y en vase a eso calcular los mililitros de agua y gramos de alimento y accionar las
 * valvulas si es necesario
 * @param[in] pvParameter void* que corresponde a los parametros de la tarea
 */
static void MedirLlenarTask(void *pvParameter){
	uint16_t valorCH1 = 0;
    while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //La tarea espera en este punto hasta recibir una notificación
		
		if (tecON) //Si tecON == 1 se mide
		{
			//Mediciones
			distancia = HcSr04ReadDistanceInCentimeters(); // Mido la distancia a la superficie del agua
			AnalogInputReadSingle(CH1, &valorCH1); //Leo milivolts correspondientes al peso del alimento

			//Calculo de mililitros de agua
			mlagua= 3.14 * 10 * 10 *(30-distancia);

			//Calculo de gramos de alimento
			peso=valorCH1/3.3; //Vin [mV] = 3.3 * peso [g] => peso [g] = Vin [mV] / 3.3

			//Accion valvula de agua
			if(mlagua < 2500){ //Si hay menos de 2500 ml de agua se puede seguir llenando el recipiente de agua
				if(mlagua < 500){ //Si hay menos de 500 ml de agua se abre la valvula de agua
					GPIOOn(GPIO_23);
				}	
			}
			else{
				GPIOOff(GPIO_23);
			}

			//Accion valvula de alimento
			if(peso < 500){ //Si hay menos de 500 gramos de alimento se puede seguir llenando el recipiente de alimento
				if(peso < 50){ //Si hay menos de 100 gramos de alimento se abre la valvula de alimento	
					GPIOOn(GPIO_2);
				}
			}
			else{
				GPIOOff(GPIO_2);
			}
		}
    }
}

/** @def static void ComunicarTask(void *pvParameter)
 * @brief Tarea encargada de comunicar por UART a la PC el estado de los recipientes, el peso
 * de alimento y los mililitros de agua
 * @param[in] pvParameter void* que corresponde a los métodos de la tarea
 */
static void ComunicarTask(void *pvParameter){
	uint16_t dmostrar = 0;
    while(true)
	{	
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //La tarea espera en este punto hasta recibir una notificación
		
		if(tecON) //Si tecON == 1 se comunica el estado de los recipientes
		{
			UartSendString(UART_PC, "Agua: \r"); //Mando el string por UART
			UartSendString(UART_PC, (char*)UartItoa(mlagua, 10)); //Mando el string por UART
            UartSendString(UART_PC, " cm^3, Alimento: \r"); //Mando el string por UART
			UartSendString(UART_PC, (char*)UartItoa(peso, 10)); //Mando el string por UART
			UartSendString(UART_PC, " gr\r\n"); //Mando el string por UART
		}		
    }
}

/*==================[external functions definition]==========================*/
void app_main(void){
	
	/*Inicializations*/
	LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2); //Medidor de distancia por ultrasonido
	GPIOInit(GPIO_23, GPIO_OUTPUT); //GPIO para la electrovalvula del agua
	GPIOInit(GPIO_2, GPIO_OUTPUT); //GPIO para la electrovalvula del alimento

	timer_config_t timer_medicion = { //Timer para medir los recipientes y rellenar
        .timer = TIMER_A,
        .period = CONFIG_MEASURE_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_medicion);

	timer_config_t timer_comunicacion = { //Timer para comunicar mililitros de agua y gramos de alimento
        .timer = TIMER_B,
        .period = CONFIG_COMUNICATION_PERIOD,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
    TimerInit(&timer_comunicacion);

	serial_config_t puertoSerie={ //Puerto serie para comunicarse con la PC por UART
		.port =	UART_PC,
		.baud_rate = 9600,
		.func_p = NULL,
		.param_p = NULL
	};
	UartInit(&puertoSerie);

	analog_input_config_t analogConverter={ //ADC para medir el peso del alimento
		.input = CH1,			
		.mode = ADC_SINGLE,			
		.func_p = NULL,				
		.param_p = NULL,		
		.sample_frec = NULL		
	};
	AnalogInputInit(&analogConverter);

	/*tasks*/
	SwitchActivInt(SWITCH_1, OnOffSwitch, NULL); //Para prender y apagar el dispositivo
	xTaskCreate(&MedirLlenarTask, "MedirLlenar", 512, NULL, 5, &medirllenar_task_handle); //Mide los recipientes y los llena segun el caso
	xTaskCreate(&ComunicarTask, "Comunicar", 512, NULL, 5, &comunicar_task_handle); //Comunica el estado de los recipientes

	/*timers start*/
    TimerStart(timer_medicion.timer);
    TimerStart(timer_comunicacion.timer);

	LedsOffAll();

}
/*==================[end of file]============================================*/