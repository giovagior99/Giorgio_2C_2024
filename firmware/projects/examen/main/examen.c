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
/*==================[macros and definitions]=================================*/

/** @def CONFIG_MEDIR_PERIOD
 * @brief Periodo de tiempo de refresco de medición del HC-SR04 (500 ms)
 */
#define CONFIG_MEDIR_PERIOD 500000 //500 ms en microsegundos

/** @def CONFIG_NOTIFICAR_PERIOD
 * @brief Periodo de tiempo de refresco de notificacion en leds y buzzer (500 ms)
 */
#define CONFIG_NOTIFICAR_PERIOD 500000 //500 ms en microsegundos

/** @def CONFIG_BUZZER_PERIOD
 * @brief Periodo de tiempo de delay para el buzzer
 */
#define CONFIG_BUZZER_PERIOD 500 //500 ms

/*==================[internal data definition]===============================*/

/** @def medir_task_handle
 *  @brief
 */
TaskHandle_t medir_task_handle = NULL;

/** @def mostrar_task_handle
 * @brief
 */
TaskHandle_t notificar_task_handle = NULL;

/** @def comunicar_task_handle
 * @brief
 */
TaskHandle_t comunicar_task_handle = NULL;

/** @def distancia
 * @brief Distancia medida por el sensor HC-SR04
 */
uint16_t distancia = 0;

/*==================[internal functions declaration]=========================*/

void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE);
}

/**  @def void FuncTimerB(void* param)
 * @brief Función invocada en la interrupción del timer B
 */
void FuncTimerB(void* param){
   vTaskNotifyGiveFromISR(notificar_task_handle, pdFALSE); 
}

/** @def static void MedirTask(void *pvParameter)
 * @brief Tarea encargada de medir la distancia cada cierto periodo de tiempo
 * @param[in] pvParameter void* que corresponde a los parametros de la tarea
 */
static void MedirTask(void *pvParameter){
    while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */
		distancia = HcSr04ReadDistanceInCentimeters();
	}
}

/** @def static void NotificarTask(void *pvParameter)
 * @brief Tarea encargada de mostrar la distancia en los leds y activar el buzzer.
 * Si distancia >= 5m enciente el led verde
 * Si 3m <= distancia < 5m enciende los leds verde y amarillo
 * Si distancia < 3m enciende los leds verde, amarillo y rojo
 * Activa el buzzer si distancia < 5m, con una frecuancia de 1s si distancia < 5m y 0.5s
 * si distancia < 3m
 * @param[in] pvParameter void* que corresponde a los métodos de la tarea
 */
static void NotificarTask(void *pvParameter){
    while(true)
	{	
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    /* La tarea espera en este punto hasta recibir una notificación */

		LedsOffAll();
		GPIOOff(GPIO_23);

		if(distancia < 300) {
    		LedOn(LED_1);
    		LedOn(LED_2);
    		LedOn(LED_3);
			GPIOOn(GPIO_23);
    	}
		if((distancia >= 300) && (distancia < 500)) 
		{
    		LedOn(LED_1);
    		LedOn(LED_2);
			GPIOOn(GPIO_23);
			vTaskDelay(CONFIG_BUZZER_PERIOD / portTICK_PERIOD_MS);
    	}
    	if(distancia >= 500)
		{
    		LedOn(LED_1);
    	}
    }
}

/*==================[external functions definition]==========================*/
void app_main(void){

	/* initializations */
	HcSr04Init(GPIO_3, GPIO_2);
	LedsInit();
	GPIOInit(GPIO_23, GPIO_OUTPUT); //GPIO para el Buzzer

	timer_config_t timer_medicion = {
        .timer = TIMER_A,
        .period = CONFIG_MEDIR_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_medicion);

	timer_config_t timer_notificacion = {
        .timer = TIMER_B,
        .period = CONFIG_NOTIFICAR_PERIOD,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
    TimerInit(&timer_notificacion);

	/*tasks*/
    xTaskCreate(&MedirTask, "Medir", 512, NULL, 5, &medir_task_handle);
	xTaskCreate(&NotificarTask, "Notificar", 512, NULL, 5, &notificar_task_handle);

	/*timers start*/
    TimerStart(timer_medicion.timer);
    TimerStart(timer_notificacion.timer);

}
/*==================[end of file]============================================*/