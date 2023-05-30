/*
 * adc.c
 *
 *  Created on: 30 may. 2023
 *      Author: Álvaro Leva
 */

#include "components/stm32main.h"
#include "components/formulas.h"

double get_Icell(void){

	//config. to read Icell (PA0)
	sConfig.Channel = ADC_CHANNEL_0; // Configure ADC channel for PA0
	sConfig.Rank = 1; //set the rank to 1, to read first and uniquely Icell
	HAL_ADC_ConfigChannel(&hadc1, &sConfig); //update the configuration

	//get the ADC measure
	HAL_ADC_PollForConversion(&hadc1, 200);// wait the conversion to end
	adc_Icell = HAL_ADC_GetValue(&hadc1);
	Icell = calculateIcellCurrent(adc_Icell);

	return  Icell;

}

double get_Vcell(void){

	//config. to read Vcell (PA1)
	sConfig.Channel = ADC_CHANNEL_1; // Configure ADC channel for PA1
	sConfig.Rank = 1; //set the rank to 1, to read first and uniquely Vcell
	HAL_ADC_ConfigChannel(&hadc1, &sConfig); //update the configuration

	//get the ADC measure
	HAL_ADC_PollForConversion(&hadc1, 200);// wait the conversion to end
	adc_Vcell = HAL_ADC_GetValue(&hadc1);
	Vcell = calculateVrefVoltage(adc_Vcell);

	return  Vcell;

}

