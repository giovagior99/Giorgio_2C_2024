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
 * | 	  ECHO	 	| 	 GPIO_3		|
 * | 	 TRIGGER 	| 	 GPIO_2		|
 * |      +5V	 	| 	  +5V		|
 * | 	  GND	 	| 	  GND		|
 *
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

/*==================[macros and definitions]=================================*/

/** @def CONFIG_MEASURE_PERIOD
 * @brief Periodo de tiempo de refresco de medición de los recipientes y accionamiento
 * de las valvulas
 */
#define CONFIG_MEASURE_PERIOD 500000 //en microsegundos

/** @def CONFIG_SHOW_PERIOD
 * @brief Periodo de tiempo para comunicar la altura de agua en el recipiente y gramos
 * de alimento
 */
#define CONFIG_COMUNICATION_PERIOD 500000 //en microsegundos

/*==================[internal data definition]===============================*/

/** @def tecON
 * @brief Booleano para encender o apagar el dispositivo
 */
bool tecON = 0; //Por defecto esta apagado

/** @def medir_task_handle
 *  @brief -
 */
TaskHandle_t medirllenar_task_handle = NULL;

/** @def mostrar_task_handle
 * @brief -
 */
TaskHandle_t comunicar_task_handle = NULL;

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
 * @brief Tarea encargada de medir 
 * @param[in] pvParameter void* que corresponde a los parametros de la tarea
 */
static void MedirLlenarTask(void *pvParameter){
    while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    /* La tarea espera en este punto hasta recibir una notificación */
		
		if (tecON) //Si tecON == 1 se mide
		{
			
		}
        
    }
}

/** @def static void ComunicarTask(void *pvParameter)
 * @brief Tarea encargada de 
 * @param[in] pvParameter void* que corresponde a los métodos de la tarea
 */
static void ComunicarTask(void *pvParameter){
	uint16_t dmostrar = 0;
    while(true)
	{	
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    /* La tarea espera en este punto hasta recibir una notificación */
		
		if(tecON) //Si tecON == 1 se muestra la distancia
		{

		}
			
    }
}

/*==================[external functions definition]==========================*/
void app_main(void){
	
	/*Inicializations*/
	LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2); //Medidor de distancia por ultrasonido

	timer_config_t timer_medicion = { //Timer para medir los recipientes y rellenar
        .timer = TIMER_A,
        .period = CONFIG_MEASURE_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_medicion);

	timer_config_t timer_comunicacion = { //Timer para comunicar la altura y gramos en los recipientes
        .timer = TIMER_B,
        .period = CONFIG_COMUNICATION_PERIOD,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
    TimerInit(&timer_comunicacion);

	/*tasks*/
	SwitchActivInt(SWITCH_1, OnOffSwitch, NULL); //Para prender y apagar el dispositivo
	xTaskCreate(&MedirLlenarTask, "MedirLlenar", 512, NULL, 5, &medirllenar_task_handle);
	xTaskCreate(&ComunicarTask, "Comunicar", 512, NULL, 5, &comunicar_task_handle);

	/*timers start*/
    TimerStart(timer_medicion.timer);
    TimerStart(timer_comunicacion.timer);


	LedsOffAll();

}
/*==================[end of file]============================================*/