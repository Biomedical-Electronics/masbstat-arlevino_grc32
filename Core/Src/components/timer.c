/*
 * timer.c
 *
 *  Created on: 30 may. 2023
 *      Author: USER
 */

#include "components/stm32main.h"

volatile _Bool timer;

void set_up_Timer(double samplingPeriodMs){

	__HAL_TIM_SET_COUNTER(&htim2, 0); //set counter to 0
	__HAL_TIM_SET_AUTORELOAD(&htim2, samplingPeriodMs); //set the counter samplingPeriod
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	timer = TRUE;

}

