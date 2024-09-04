/*! @mainpage Guia2_Ej2
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |    ESP32   	|
 * |:--------------:|:--------------|
 * | 	  ECHO	 	| 	 GPIO_3		|
 * | 	 TRIGGER 	| 	 GPIO_2		|
 * |      +5V	 	| 	  +5V		|
 * | 	  GND	 	| 	  GND		|
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 04/09/2024 | Document creation		                         |
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
#include "switch.h"
#include "lcditse0803.h"

/*==================[macros and definitions]=================================*/
/** @def CONFIG_MODE_PERIOD
 * @brief Periodo de tiempo de refresco de modo (teclas encendido o hold)
 */
#define CONFIG_MODE_PERIOD 100

/** @def CONFIG_MEASURE_PERIOD
 * @brief Periodo de tiempo de refresco de medición
 */
#define CONFIG_MEASURE_PERIOD 1000

/** @def CONFIG_SHOW_PERIOD
 * @brief Periodo de tiempo de refresco de mostrar
 */
#define CONFIG_SHOW_PERIOD 500

/*==================[internal data definition]===============================*/
/** @def tecON
 * @brief Booleano para encender o apagar la medición
 */
bool tecON = 0; //Por defecto esta apagado

/** @def tecHOLD
 * @brief Booleano para mantener la medicion en el display
*/
bool tecHOLD = 0; //Por defecto no mantiene la medición

/** @def distancia
 * @brief Distancia medida
 */
uint16_t distancia = 0;

/** @def modo_task_handle
 * @brief 
 */
TaskHandle_t modo_task_handle = NULL;

/** @def medir_task_handle
 *  @brief
 */
TaskHandle_t medir_task_handle = NULL;

/** @def mostrar_task_handle
 * @brief
 */
TaskHandle_t mostrar_task_handle = NULL;

/*==================[internal functions declaration]=========================*/
/** @def static void ModoTask(void *pvParameter)
 * @brief Tarea encargada de leer los cambios en los switches para configurar los modos
 * On o Hold
 * @param[in] pvParameter void* que corresponde a los parametros de la tarea
 */
static void ModoTask(void *pvParameter){
	bool boton = 0; //Booleano que almacena el switch presionado
    while(true){
        boton = SwitchesRead();
    	switch(boton){ //Ve si se debe encender la medicion y si se debe mantener
    		case SWITCH_1:
    			tecON = !tecON;
    			break;
    		case SWITCH_2:
    			tecHOLD = !tecHOLD;
    			break;
    		default:
    			break;
    	}
		vTaskDelay(CONFIG_MODE_PERIOD / portTICK_PERIOD_MS);
    }
}

/** @def static void MedirTask(void *pvParameter)
 * @brief Tarea encargada de medir la distancia cada cierto periodo de tiempo
 * @param[in] pvParameter void* que corresponde a los parametros de la tarea
 */
static void MedirTask(void *pvParameter){
    while(true)
	{
		if (tecON) //Si tecON == 1 se mide
		{
			distancia = HcSr04ReadDistanceInCentimeters();
		}
        vTaskDelay(CONFIG_MEASURE_PERIOD / portTICK_PERIOD_MS);
    }
}

/** @def static void MostrarTask(void *pvParameter)
 * @brief Tarea encargada de mostrar la distancia en el display y leds dependiendo del estado de On y Hold.
 * Si tecON == 0 no se muestra nada
 * Si tecON == 1 se muestra la distancia
 * Si tecHOLD == 0 se actualiza la distancia mostrada
 * Si tecHOLD == 1 se mantiene lo mostrado
 * @param[in] pvParameter void* que corresponde a los métodos de la tarea
 */
static void MostrarTask(void *pvParameter){
	uint16_t dmostrar = 0;
    while(true)
	{	
		if(tecON) //Si tecON == 1 se muestra la distancia
		{
			if(!tecHOLD)
			{ 
				dmostrar = distancia; //Si tecHOLD == 0 se actualiza la distancia a mostrar
    		}

			LcdItsE0803Write(dmostrar);

			LedsOffAll();

			if((dmostrar > 0) && (dmostrar <= 10))
			{
    			LedsOffAll();
    		}
    		if((dmostrar > 10) && (dmostrar <= 20))
			{
    			LedOn(LED_1);
    		}
    		if((dmostrar > 20) && (dmostrar <= 30)) 
			{
    			LedOn(LED_1);
    			LedOn(LED_2);
    		}
    		if(dmostrar > 30) {
    			LedOn(LED_1);
    			LedOn(LED_2);
    			LedOn(LED_3);
    		}
		}
		vTaskDelay(CONFIG_SHOW_PERIOD / portTICK_PERIOD_MS);
    }
}
/*==================[external functions definition]==========================*/
void app_main(void){

	/* initializations */
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();
	LedsInit();

	/*tasks*/
	xTaskCreate(&ModoTask, "ModoOnHold", 512, NULL, 5, &modo_task_handle);
    xTaskCreate(&MedirTask, "Medir", 512, NULL, 5, &medir_task_handle);
	xTaskCreate(&MostrarTask, "Mostrar", 512, NULL, 5, &mostrar_task_handle);
    
}
/*==================[end of file]============================================*/