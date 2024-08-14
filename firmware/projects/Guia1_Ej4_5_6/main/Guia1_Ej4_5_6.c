/*! @mainpage Guia1_Ej4_5_6
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
 * | 14/08/2024 | Document creation		                         |
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

/*==================[macros and definitions]=================================*/
#define MASC_BIT_1 1

typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

gpioConf_t pinBCD[4]={{GPIO_20,GPIO_OUTPUT},{GPIO_21,GPIO_OUTPUT},{GPIO_22,GPIO_OUTPUT},{GPIO_23,GPIO_OUTPUT}};
gpioConf_t pinBCD_LCD[3]={{GPIO_19,GPIO_OUTPUT},{GPIO_18,GPIO_OUTPUT},{GPIO_9,GPIO_OUTPUT}};

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	uint32_t aux = data;

	for(int i=digits;i>0;i--)
	{
		bcd_number[i-1] = aux % 10;

		aux = aux / 10;
	}

	return bcd_number;
}

void BcdToGPIO(uint8_t digit, gpioConf_t * pins_vector)
{
	for(int i=0;i<4;i++)
	{
		if(digit & (MASC_BIT_1<<i)) //realiza un corrimiento
		{
			GPIOOn(pins_vector[i].pin);
		}
		else
		{
			GPIOOff(pins_vector[i].pin);
		}
	}
}

void BcdToLCD(uint32_t numero, uint8_t n_digitos, gpioConf_t * pins_vector, gpioConf_t * pins_LCD)
{
	uint8_t vector[n_digitos];

	convertToBcdArray(numero, n_digitos, vector);

	for(int i=0;i<n_digitos;i++)
	{
		printf("%d\n",vector[i]);
		BcdToGPIO(vector[i],pins_vector);
		GPIOOn(pins_LCD[i].pin);
		
		GPIOOff(pins_LCD[i].pin);
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){
	
	/* initializations */
 	uint32_t numero = 645;
	uint8_t num_digitos = 3;
	uint8_t vector[3];

	for(int i=0;i<4;i++)
	{
		GPIOInit(pinBCD[i].pin, pinBCD[i].dir);
	}

	for(int i=0;i<3;i++)
	{
		GPIOInit(pinBCD_LCD[i].pin, pinBCD_LCD[i].dir);
	}
	
	//convertToBcdArray(numero, num_digitos, vector);

	/*printf("El número %ld convertido a BCD es: ", numero);
	for (int i = 0; i < num_digitos; i++) {
    	printf("%X ", vector[i]);
	}*/

	//BcdToGPIO(vector[2],pinBCD);

	BcdToLCD(numero, num_digitos, pinBCD, pinBCD_LCD);
}
/*==================[end of file]============================================*/