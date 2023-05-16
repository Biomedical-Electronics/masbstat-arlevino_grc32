/*
 * PMU.c
 *
 *  Created on: 16 may. 2023
 *      Author: Leva Álvaro
 *      Definiremos las funciones de control de la PMU:
 *      - PMU_init
 *      - PMU_end
 */

#include "components/PMU.h"

//Function to initialize the PMU ( EN = 1 )
void PMU_init(void) {

	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, GPIO_PIN_SET); // EN = 1
	HAL_Delay(500); // wait 500ms
}

//Function to end the PMU ( EN = 0 )
void PMU_end(void) {

	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, GPIO_PIN_RESET); // EN = 0

}


