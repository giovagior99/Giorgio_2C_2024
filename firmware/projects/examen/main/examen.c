/*! @mainpage Examen
 *
 * @section genDesc Examen individual de la asignatura Electronica Programable
 * de la carrera Bioingenieria de la FIUNER
 *
 * This section describes how the program works.
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	  ECHO	 	| 	 GPIO_3		|
 * | 	 TRIGGER 	| 	 GPIO_2		|
 * |      LED 1	 	| 	 GPIO_11	|
 * | 	  LED 2	 	| 	 GPIO_10	|
 * |      LED 3	 	| 	 GPIO_5		|
 * |      BUZZER 	| 	 GPIO_23	|
 * |      UART TX 	| 	 GPIO_18	|
 * |      UART RX 	| 	 GPIO_19	|
 * | 	  GND	 	| 	  GND		|
 * |      +5V	 	| 	  +5V		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 04/11/2024 | Document creation		                         |
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
#include "timer_mcu.h"
#include "uart_mcu.h"
/*==================[macros and definitions]=================================*/

/** @def CONFIG_SENSOR_DISTANCIA_PERIOD
 * @brief Periodo de tiempo de refresco de medición del HC-SR04 (500 ms)
 */
#define CONFIG_SENSOR_DISTANCIA_PERIOD 500000 //500 ms en microsegundos

/** @def CONFIG_ACELEROMETRO_PERIOD
 * @brief Periodo de tiempo de refresco de notificacion en leds y buzzer (500 ms)
 */
#define CONFIG_ACELEROMETRO_PERIOD 500000 //500 ms en microsegundos

/** @def CONFIG_BUZZER_PERIOD
 * @brief Periodo de tiempo de delay para el buzzer
 */
#define CONFIG_BUZZER_PERIOD 500 //500 ms

/*==================[internal data definition]===============================*/

/** @def sensorDis_task_handle
 *  @brief
 */
TaskHandle_t sensorDis_task_handle = NULL;

/** @def mostrar_task_handle
 * @brief
 */
TaskHandle_t acelerometro_task_handle = NULL;

/** @def distancia
 * @brief Distancia medida por el sensor HC-SR04
 */
uint16_t distancia = 0;

/*==================[internal functions declaration]=========================*/

void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(sensorDis_task_handle, pdFALSE);
}

/**  @def void FuncTimerB(void* param)
 * @brief Función invocada en la interrupción del timer B
 */
void FuncTimerB(void* param){
   vTaskNotifyGiveFromISR(acelerometro_task_handle, pdFALSE); 
}

/** @def static void SensorDisTask(void *pvParameter)
 * @brief Tarea encargada de medir la distancia cada cierto periodo de tiempo, y de mostrar
 * la distancia en los leds, activar el buzzer y enviar mensaje a modulo bluetooth.
 * - Si distancia >= 5m enciente el led verde
 * - Si 3m <= distancia < 5m enciende los leds verde y amarillo, activa el buzzer cada 1s y muestra
 * el mesaje "Precaucion, vehiculo cerca"
 * - Si distancia < 3m enciende los leds verde, amarillo y rojo, activa el buzzer cada 0.5s y muestra
 * el mesaje "Peligro, vehiculo cerca"
 * @param[in] pvParameter void* que corresponde a los parametros de la tarea
 */
static void SensorDisTask(void *pvParameter){
    while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */
		distancia = HcSr04ReadDistanceInCentimeters();

		LedsOffAll();
		GPIOOff(GPIO_23);

		if(distancia < 300) {
    		LedOn(LED_1);
    		LedOn(LED_2);
    		LedOn(LED_3);
			GPIOOn(GPIO_23);
			UartSendString(UART_CONNECTOR, "Peligro, vehiculo cerca");
    	}
		if((distancia >= 300) && (distancia < 500)) 
		{
    		LedOn(LED_1);
    		LedOn(LED_2);
			GPIOOn(GPIO_23);
			UartSendString(UART_CONNECTOR, "Precaucion, vehiculo cerca");
			vTaskDelay(CONFIG_BUZZER_PERIOD / portTICK_PERIOD_MS);
    	}
    	if(distancia >= 500)
		{
    		LedOn(LED_1);
    	}
	}
}

/** @def static void AcelerometroTask(void *pvParameter)
 * @brief Tarea encargada 
 * @param[in] pvParameter void* que corresponde a los métodos de la tarea
 */
static void AcelerometroTask(void *pvParameter){
    while(true)
	{	
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    /* La tarea espera en este punto hasta recibir una notificación */

		


    }
}

/**  @def void FuncUart(void* param)
 * @brief Función invocada al recibir un dato por UART
 */
void FuncUart(void* param){
	uint8_t dato;
    UartReadByte(UART_PC, &dato);
	UartSendByte(UART_PC, (char *) &dato);
}

/*==================[external functions definition]==========================*/
void app_main(void){

	/* initializations */
	HcSr04Init(GPIO_3, GPIO_2);
	LedsInit();
	GPIOInit(GPIO_23, GPIO_OUTPUT); //GPIO para el Buzzer

	timer_config_t timer_sensorDist= {
        .timer = TIMER_A,
        .period = CONFIG_SENSOR_DISTANCIA_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_sensorDist);

	timer_config_t timer_acelerometro = {
        .timer = TIMER_B,
        .period = CONFIG_ACELEROMETRO_PERIOD,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
    TimerInit(&timer_acelerometro);

	serial_config_t puertoSerie={			
		.port =	UART_CONNECTOR,
		.baud_rate = 115200,
		.func_p = FuncUart,
		.param_p = NULL
	};
	UartInit(&puertoSerie);

	/*tasks*/
    xTaskCreate(&SensorDisTask, "SensorDistancia", 512, NULL, 5, &sensorDis_task_handle);
	xTaskCreate(&AcelerometroTask, "Acelerometro", 512, NULL, 5, &acelerometro_task_handle);

	/*timers start*/
    TimerStart(timer_sensorDist.timer);
    TimerStart(timer_acelerometro.timer);

}
/*==================[end of file]============================================*/