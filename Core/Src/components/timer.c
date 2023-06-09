/*
 * timer.c
 *
 *  Created on: 30 may. 2023
 *      Author: USER
 */

#include "components/stm32main.h"

volatile _Bool timer;


void set_up_Timer(uint32_t samplingPeriodMs){
	timer = FALSE;
	__HAL_TIM_SET_COUNTER(&htim2, 0); //set counter to 0
	__HAL_TIM_SET_AUTORELOAD(&htim2, samplingPeriodMs); //set the counter samplingPeriod
	HAL_TIM_Base_Start_IT(&htim2);
}

void stop_Timer(void){

	HAL_TIM_Base_Stop(&htim2);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	timer = TRUE;

}

