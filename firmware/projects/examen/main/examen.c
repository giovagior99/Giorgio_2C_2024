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
 * |      +5V	 	| 	  +5V		|
 * | 	  GND	 	| 	  GND		|
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
#define CONFIG_MEDIR_PERIOD 500000 //en microsegundos

/** @def CONFIG_NOTIFICAR_PERIOD
 * @brief Periodo de tiempo de refresco de notificacion en leds y buzzer (500 ms)
 */
#define CONFIG_NOTIFICAR_PERIOD 500000 //en microsegundos

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

/*==================[internal functions declaration]=========================*/

void FuncTimerA(void* param){
    
}

/**  @def void FuncTimerB(void* param)
 * @brief Función invocada en la interrupción del timer B
 */
void FuncTimerB(void* param){
   
}

/*==================[external functions definition]==========================*/
void app_main(void){

	/* initializations */
	HcSr04Init(GPIO_3, GPIO_2);
	LedsInit();

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

}
/*==================[end of file]============================================*/