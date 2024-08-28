/*! @mainpage Guia1_Ej4_5_6
 *
 * @section genDesc General Description
 *
 * Este programa descompone un numero decimal a digitos BCD, luego cada digito BCD lo convierte a binario y
 * configura salidas GPIO con los valores correspondientes al binario de cuatro digitos. Además, 
 * se muetsra por pantalla LCD el numero original mostrando las cifras del BCD.
 *
 * @section hardConn Hardware Connection
 *
 * |   Peripheral   |     ESP32   	|
 * |:--------------:|:--------------|
 * |       D1	 	| 	 GPIO_20	|
 * | 	   D2	 	| 	 GPIO_21	|
 * | 	   D3	 	|    GPIO_22	|
 * | 	   D4	 	| 	 GPIO_23	|
 * |     SEL_1	 	| 	 GPIO_19	|
 * | 	 SEL_2	 	| 	 GPIO_18	|
 * | 	 SEL_3	 	| 	 GPIO_9 	|
 * | 	  +5V	 	| 	   +5V		|
 * | 	  GND	 	| 	   GND		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 14/08/2024 | Document creation		                         |
 * | 14/08/2024 | Functionality working	                         |
 * | 28/08/2024 | Document actualization and documentation       |
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

/** @def MASC_BIT_1
 * @brief Mascara que se utilizara para encender el GPIO correspondiente.
 */
#define MASC_BIT_1 1

/** @def gpioConf_t
 * @brief Struck que configura el GPIO
 */
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/** @def pinBCD
 * @brief Vector que contiene los 4 GPIO que corresponden a los numeros BCD 
 */
gpioConf_t pinBCD[4]={{GPIO_20,GPIO_OUTPUT},{GPIO_21,GPIO_OUTPUT},{GPIO_22,GPIO_OUTPUT},{GPIO_23,GPIO_OUTPUT}};

/** @def pinBCD_LCD
 * @brief Vector que contiene los 3 GPIO que corresponden a la pantalla LCD
 */
gpioConf_t pinBCD_LCD[3]={{GPIO_19,GPIO_OUTPUT},{GPIO_18,GPIO_OUTPUT},{GPIO_9,GPIO_OUTPUT}};

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
/** @def convertToBcdArray(uint32_t data, uint8_t digits, uint8_t * bcd_number)
 * @brief Funcion que convierte numeros decimales a digitos BCD
 * @param[in] data uint32_t que corresponde al numero decimal a convertir
 * @param[in] digits uint8_t que corresponde al numero de digitos del numero a convertir
 * @param[in] bcd_number uint8_t que corresponde al vector donde se almacenan los digitos del BCD
 * @return null
 */
int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	uint32_t aux = data;

	for(int i=digits;i>0;i--)
	{
		bcd_number[i-1] = aux % 10;

		aux = aux / 10;
	}

	return 0;
}

/** @def BcdToGPIO(uint8_t digit, gpioConf_t * pins_vector)
 * @brief Funcion que convierte un digito BCD a binario
 * @param[in] digit uint8_t que corresponde al digito a convertir
 * @param[in] pins_vector gpioConf_t que corresponde al vector de GPIO que hay que configurar con el binario
 * @return null
 */
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

/** @def BcdToLCD(uint32_t numero, uint8_t n_digitos, gpioConf_t * pins_vector, gpioConf_t * pins_LCD)
 * @brief Funcio que muestra digitos BCD en la pantalla LCD
 * @param[in] numero uint32_t que corresponde al numero a mostrar en el LCD
 * @param[in] n_digitos uint8_t que corresponde al numero de digitos que tiene el numero
 * @param[in] pins_vector gpioConf_t que corresponde al vector de GPIO que hay que configurar con el binario
 * @param[in] pins_LCD gpioConf_t que corresponde al vector de GPIO que hay que configurar para la pantalla LCD
 * @return null
 */
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
	
	/* Initializations */
 	uint32_t numero = 645; // Numero a mostrar en el LCD
	uint8_t num_digitos = 3; // Numero de digitos que tiene el numero
	uint8_t vector[3]; // Vector donde se almacenan los digitos BCD del numero

	for(int i=0;i<4;i++) // Crea el vector de GPIO para los 4 digitos BCD
	{
		GPIOInit(pinBCD[i].pin, pinBCD[i].dir);
	}

	for(int i=0;i<3;i++) // Crea el vector de GPIO para la pantalla LCD
	{
		GPIOInit(pinBCD_LCD[i].pin, pinBCD_LCD[i].dir);
	}
	
	//convertToBcdArray(numero, num_digitos, vector);

	/*printf("El número %ld convertido a BCD es: ", numero);
	for (int i = 0; i < num_digitos; i++) {
    	printf("%X ", vector[i]);
	}*/

	//BcdToGPIO(vector[2],pinBCD);

	BcdToLCD(numero, num_digitos, pinBCD, pinBCD_LCD); //Muestra el numero en la pantalla LCD
}
/*==================[end of file]============================================*/