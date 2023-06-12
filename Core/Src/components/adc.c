/*
 * adc.c
 *
 *  Created on: 30 may. 2023
 *      Author: Álvaro Leva
 */

#include "components/stm32main.h"
#include "components/formulas.h"
#include "components/masb_comm_s.h"
#include "components/mcp4725_driver.h"
#include "components/chronoamperometry.h"
#include "components/timer.h"
#include "main.h"

ADC_ChannelConfTypeDef sConfig;
extern ADC_HandleTypeDef hadc1;

uint32_t adc_Vcell;
double Vcell;
uint32_t adc_Icell;
double Icell;

//extern struct Handles_S handles;

double get_Icell(void){

	//config. to read Icell (PA0)
	sConfig.Channel = ADC_CHANNEL_0; // Configure ADC channel for PA0
	sConfig.Rank = 1; //set the rank to 1, to read first and uniquely Icell
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_ADC_ConfigChannel(&hadc1, &sConfig); //update the configuration

	//get the ADC measure
	HAL_ADC_Start(&hadc1); // init adc
	HAL_ADC_PollForConversion(&hadc1, 200);// wait the conversion to end
	adc_Icell = HAL_ADC_GetValue(&hadc1);

	Icell = calculateIcellCurrent(adc_Icell);

	return  Icell;
}

double get_Vcell(void){

	//config. to read Vcell (PA1)
	sConfig.Channel = ADC_CHANNEL_1; // Configure ADC channel for PA1
	sConfig.Rank = 1; //set the rank to 1, to read first and uniquely Vcell
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_ADC_ConfigChannel(&hadc1, &sConfig); //update the configuration

	//get the ADC measure
	HAL_ADC_Start(&hadc1); // init adc
	HAL_ADC_PollForConversion(&hadc1, 200);// wait the conversion to end
	adc_Vcell = HAL_ADC_GetValue(&hadc1);
	Vcell = calculateVrefVoltage(adc_Vcell);

	return  Vcell;

}

