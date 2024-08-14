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
//gpioConf_t pinBCD[4]={{GPIO_LCD_1,GPIO_OUTPUT},{GPIO_LCD_2,GPIO_OUTPUT},{GPIO_LCD_3,GPIO_OUTPUT},{GPIO_LCD_4,GPIO_OUTPUT}};
//gpioConf_t pinMUX[3]={{GPIO_1,GPIO_OUTPUT},{GPIO_2,GPIO_OUTPUT},{GPIO_5,GPIO_OUTPUT}};

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	uint32_t aux = data;
	uint8_t control[3];

	for(int i=digits;i>0;i--)
	{
		bcd_number[i-1] = aux % 10;
		control[i-1] = aux % 10;

		aux = aux / 10;
	}

	return 1;
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


/*==================[external functions definition]==========================*/
void app_main(void){
	
	/* initializations */
 	uint32_t numero = 123;
	uint8_t num_digitos = 3;
	uint8_t vector[3];

	for(int i=0;i<4;i++)
	{
		GPIOInit(pinBCD[i].pin, pinBCD[i].dir);
	}
	
	convertToBcdArray(numero, num_digitos, vector);

	/*printf("El número %ld convertido a BCD es: ", numero);
	for (int i = 0; i < num_digitos; i++) {
    	printf("%X ", vector[i]);
	}*/

	BcdToGPIO(vector[2],pinBCD);
}
/*==================[end of file]============================================*/