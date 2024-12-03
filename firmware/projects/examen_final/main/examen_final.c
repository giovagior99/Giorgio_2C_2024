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

/*==================[internal functions declaration]=========================*/

/** @def void FuncTimerA(void* param)
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerA(void* param){

}

/** @def void FuncTimerB(void* param)
 * @brief Función invocada en la interrupción del timer B
 */
void FuncTimerB(void* param){

}

/*==================[external functions definition]==========================*/
void app_main(void){
	
	/*Inicializations*/
	LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);

	timer_config_t timer_medicion = {
        .timer = TIMER_A,
        .period = CONFIG_MEASURE_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_medicion);

	timer_config_t timer_comunicacion = {
        .timer = TIMER_B,
        .period = CONFIG_COMUNICATION_PERIOD,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
    TimerInit(&timer_comunicacion);

	LedsOffAll();




}
/*==================[end of file]============================================*/