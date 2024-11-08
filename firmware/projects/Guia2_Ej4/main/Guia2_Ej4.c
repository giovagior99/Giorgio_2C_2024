/*! @mainpage Template
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
 * | 22/10/2024 | Document creation		                         |
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
#include "switch.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"

/*==================[macros and definitions]=================================*/

/** @def CONFIG_INPUT_PERIOD
 * @brief Frecuencia de muestreo de 500 Hz, dispara el timer para la conversión AD
 */
#define CONFIG_INPUT_PERIOD 2000

/** @def CONFIG_OUTPUT_PERIOD
 * @brief Frecuencia de muestreo de 250 Hz, dispara el timer para la conversión DA
 */
#define CONFIG_OUTPUT_PERIOD 4000

/** @def BUFFER_SIZE
 * @brief Tamanio del buffer del ecg
 */
#define BUFFER_SIZE 231

/*==================[internal data definition]===============================*/

/** @def frecuencia
 * @brief Frecuencia de muesteo en microsegundos
 */
uint16_t frecuencia = 2000;

/** @def convertirAD_task_handle
 *  @brief
 */
TaskHandle_t convertirAD_task_handle = NULL;

/** @def convertirDA_task_handle
 *  @brief
 */
TaskHandle_t convertirDA_task_handle = NULL;

/** @def ecg
 * @brief Buffer que almacena los datos del ecg
 */
const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};

/*==================[internal functions declaration]=========================*/

/** @def void IncreaseFrecuencySwitch(void *pvParameter)
 * @brief Funcion que cuando se presiona el Switch 1 aumenta la frecuencia en 100 us
 * @param [in] NULL
*/
void IncreaseFrecuencySwitch(void *pvParameter){
	frecuencia+=100;	
}

/** @def void DecreaseFrecuencySwitch(void *pvParameter)
 * @brief Funcion que cuando se presiona el Switch 1 disminuye la frecuencia en 100 us
 * @param [in] NULL
*/
void DecreaseFrecuencySwitch(void *pvParameter){
	frecuencia-=100;	
}

/** @def static void  ConvertirDATask(void *pvParameter)
 * @brief Tarea encargada de mostrar los datos del ecg por salida digital
 * @param[in] pvParameter void* que corresponde a los parametros de la tarea
 */
static void ConvertirDATask(void *pvParameter){
	uint8_t contador = 0;
	//char buffer[20];
	while(1){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */

		if(contador < BUFFER_SIZE){
			AnalogOutputWrite((uint8_t*) ecg[contador]);
			contador++;
		}
		else{
			contador = 0;
			AnalogOutputWrite((uint8_t*) ecg[contador]);
			contador++;
		}
	}
}

/** @def static void  ConvertirADTask(void *pvParameter)
 * @brief Tarea encargada de adquirir la entrada analogica cada cierto tiempo
 * @param[in] pvParameter void* que corresponde a los parametros de la tarea
 */
static void ConvertirADTask(void *pvParameter){
	uint16_t valor = 0;
	//char buffer[20];
	while(1){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */

		AnalogInputReadSingle(CH1, &valor);

		UartSendString(UART_PC, (char*)UartItoa(valor, 10)); //Mando el string por UART
    	UartSendString(UART_PC, "\r"); //Mando el string por UART

		// sprintf(buffer, "%d\r", valor);
		// UartSendString(UART_PC, buffer); //Mando el string por UART
	}
}

/** @def void FuncTimerA(void* param)
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(convertirAD_task_handle, pdFALSE);
}

/**  @def void FuncUart(void* param)
 * @brief Función invocada al recibir un dato por UART
 */
void FuncUart(void* param){
	uint8_t dato;
    UartReadByte(UART_PC, &dato);
	UartSendByte(UART_PC, (char *) &dato);

	switch(dato){
	case 'T':
		frecuencia+=100;
		break;
	case 'B':
		frecuencia-=100;
		break;
	case 'R':
		frecuencia=2000;
		break;
	default:
		break;
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){
	/* initializations */
	SwitchesInit();

	timer_config_t timer_conversionAD = {
        .timer = TIMER_A,
        .period = CONFIG_INPUT_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_conversionAD);

	timer_config_t timer_conversionDA = {
    	.timer = TIMER_B,
        .period = CONFIG_OUTPUT_PERIOD,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
    TimerInit(&timer_conversionDA);

	serial_config_t puertoSerie={			
		.port =	UART_PC,
		.baud_rate = 115200,
		.func_p = FuncUart,
		.param_p = NULL
	};
	UartInit(&puertoSerie);
	
	analog_input_config_t analogConverter={			
		.input = CH1,			
		.mode = ADC_SINGLE,			
		.func_p = NULL,				
		.param_p = NULL,		
		.sample_frec = NULL		
	};
	AnalogInputInit(&analogConverter);

	AnalogOutputInit();

	/*tasks*/
	SwitchActivInt(SWITCH_1, IncreaseFrecuencySwitch, NULL);
	SwitchActivInt(SWITCH_2, DecreaseFrecuencySwitch, NULL);
    xTaskCreate(&ConvertirADTask, "ConvertirAD", 512, NULL, 5, &convertirAD_task_handle);
	xTaskCreate(&ConvertirDATask, "ConvertirDA", 512, NULL, 5, &convertirDA_task_handle);

	/*timers start*/
    TimerStart(timer_conversionAD.timer);
}
/*==================[end of file]============================================*/