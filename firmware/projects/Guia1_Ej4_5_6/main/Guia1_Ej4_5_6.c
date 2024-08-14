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
/*==================[macros and definitions]=================================*/

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

/*==================[external functions definition]==========================*/
void app_main(void){
	
	/* initializations */
 	uint32_t numero = 123;
	uint8_t num_digitos = 3;
	uint8_t vector[3];
	
	convertToBcdArray(numero, num_digitos, vector);


	//printf("El numero %ld convertido a BCD es", numero);

	printf("El número %ld convertido a BCD es: ", numero);
	for (int i = 0; i < num_digitos; i++) {
    	printf("%X ", vector[i]);
	}

}
/*==================[end of file]============================================*/