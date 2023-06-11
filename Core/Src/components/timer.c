/*
 * timer.c
 *
 *  Created on: 30 may. 2023
 *      Author: USER
 */

#include "components/stm32main.h"
#include "components/masb_comm_s.h"
#include "components/adc.h"
#include "main.h"

extern TIM_HandleTypeDef htim2;

volatile _Bool timer = FALSE;

void set_up_Timer(uint32_t samplingPeriodMs){
	timer = FALSE;

	__HAL_TIM_SET_COUNTER(&htim2, 0); //set counter to 0
	htim2.Instance->ARR = 84000*samplingPeriodMs;
	HAL_TIM_Base_Start_IT(&htim2);


}

void stop_Timer(void){

	HAL_TIM_Base_Stop_IT(&htim2);
	__HAL_TIM_SET_COUNTER(&htim2, 0);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
	timer = TRUE;

}

