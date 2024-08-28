/*! @mainpage Gui1_Ej3
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
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

/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 100 //100ms como período de tiempo para la función de retardo
#define ON 1
#define OFF 0
#define TOGGLE 2

/*==================[internal data definition]===============================*/
typedef struct
{
    uint8_t mode;       //ON, OFF, TOGGLE
	uint8_t n_led;      //indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de encendido/apagado
	uint16_t periodo;   //indica el tiempo de cada ciclo
} leds; 

/*==================[internal functions declaration]=========================*/
void myLeds(leds l)
{
	uint8_t i, j;

	if(l.mode == ON)
	{
		if(l.n_led == 1)
		{
			LedOn(LED_1);
		}
		if(l.n_led == 2)
		{
			LedOn(LED_2);
		}
		if(l.n_led == 3)
		{
			LedOn(LED_3);
		}
	}

	if(l.mode == OFF)
	{
		if(l.n_led == 1)
		{
			LedOff(LED_1);
		}
		if(l.n_led == 2)
		{
			LedOff(LED_2);
		}
		if(l.n_led == 3)
		{
			LedOff(LED_3);
		}
	}

	if(l.mode == TOGGLE)
	{
		for (i = 0; i < l.n_ciclos; i++)
		{
			if(l.n_led == 1)
			{
				LedToggle(LED_1);
			}
			if(l.n_led == 2)
			{
				LedToggle(LED_2);
			}
			if(l.n_led == 3)
			{
				LedToggle(LED_3);
			}

			for(j = 0; j < (l.periodo/CONFIG_BLINK_PERIOD); j++)
			vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
		}
	}

	return;
}

/*==================[external functions definition]==========================*/
void app_main(void){

	/*Inicializations*/
	uint8_t teclas;
	LedsInit();
	SwitchesInit();

	leds l;
	l.mode = TOGGLE; //ON, OFF, TOGGLE
	l.n_ciclos = 10;
	l.periodo = 500;
	l.n_led = 1;

    while(1)
	{
    	teclas  = SwitchesRead();
    	switch(teclas){
    		case SWITCH_1:
    			l.n_led = 1;
    		break;
    		case SWITCH_2:
    			l.n_led = 2;
    		break;
			case (SWITCH_1 + SWITCH_2):
    			l.n_led = 3;
    		break;
    	}

		myLeds(l);

	}

}

/*==================[end of file]============================================*/