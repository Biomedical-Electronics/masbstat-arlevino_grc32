/*
 * stm32main.h
 *
 *  Created on: 23 may. 2023
 *      Author: willy
 */

#ifndef INC_COMPONENTS_STM32MAIN_H_
#define INC_COMPONENTS_STM32MAIN_H_

#include "stm32f4xx_hal.h"

struct Handles_S {
     ADC_HandleTypeDef *hadc;
     I2C_HandleTypeDef *hi2c;
     TIM_HandleTypeDef *htim;
     UART_HandleTypeDef *huart;
};

// Prototypes.
void setup(struct Handles_S *handles);
void loop(void);

#endif /* INC_COMPONENTS_STM32MAIN_H_ */
