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
#include "led.h"

/*==================[macros and definitions]=================================*/
//#define COUNT_DELAY 3000000
#define COUNT_DELAY 30000 //se dismunuye la variable para acortar la espera
#define ON 1
#define OFF 0
#define TOGGLE 2

enum LED_MODE {one=3, off, toggle};

/*==================[internal data definition]===============================*/
typedef struct
{
    uint8_t mode;       //ON, OFF, TOGGLE
	uint8_t n_led;      //indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de encendido/apagado
	uint16_t periodo;   //indica el tiempo de cada ciclo
} leds; 

/*==================[internal functions declaration]=========================*/
void Delay(uint8_t j)
{
	uint32_t i;

	for(i=(COUNT_DELAY*j); i!=0; i--)
	{
		   asm  ("nop");
	}
}

void myLeds(leds *l)
{
	if(*l->mode == ON)
	{
		if(*l->n_led == 1)
		{
			LedOn(LED_1);
		}
		if(*l->n_led == 2)
		{
			LedOn(LED_2);
		}
		if(*l->n_led == 3)
		{
			LedOn(LED_3);
		}
	}

	if(*l->mode == OFF)
	{
		if(*l->n_led == 1)
		{
			LedOff(LED_1);
		}
		if(*l->n_led == 2)
		{
			LedOff(LED_2);
		}
		if(*l->n_led == 3)
		{
			LedOff(LED_3);
		}
	}

	if(*l->mode == TOGGLE)
	{
		if(j < *l->n_ciclos)
		{

		}
	}

	return;
}

/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();

    while(1)
	{
    	LedOn(LED_1);
		Delay();
		LedOff(LED_1);
		Delay();
	}
    

}

/*==================[end of file]============================================*/