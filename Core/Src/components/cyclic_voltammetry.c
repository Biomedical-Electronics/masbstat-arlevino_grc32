/*
 * cyclic_voltammetry.c
 *
 *  Created on: 23 may. 2023
 *      Author: Alvaro Leva
 */

#include "components/stm32main.h"
#include "components/cyclic_voltammetry.h"
#include "components/masb_comm_s.h"
#include "components/formulas.h"
#include "components/mcp4725_driver.h"

volatile _Bool timer = FALSE;
extern MCP4725_Handle_T hdac;


void make_CV(struct CV_configuration_S CV_config) {

	uint32_t point = 0;
	uint32_t time_counter = 0;

	//Extract the CV parameters from the input structure
	double eBegin = CV_config.eBegin;
	double eVertex1 = CV_config.eVertex1;
	double eVertex2 = CV_config.eVertex2;
	uint8_t cycles = CV_config.cycles;
	double scanRate = CV_config.scanRate;
	double eStep = CV_config.eStep;

	//set the desired voltage of Vcell to eBegin using the DAC
	MCP4725_SetOutputVoltage(hdac, calculateDacOutputVoltage(eBegin));

	//set the objective voltage to eVertex1
	vObjective = eVertex;

	//close reley -> Set RELAY GPIO to 1
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET); // RELAY = 1

	//define the sampling period -> time at which the timer must trigger the ISR to toggle "timer"
	samplingPeriodMs = eStep / scanRate;

	__HAL_TIM_SET_AUTORELOAD(&htim2, samplingPeriodMs);

	//Funcion ISR del timer del archivo del Leva que cambia una variable a True
	//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)

	//conduct the measures after every sampling period
	while (time_counter < measurementTimeMs){
		if(timer){ //if timer is True (samplingperiodMs has passed)
			timer = FALSE; //set the variable at false again so the loop wont happen forever

			//medir Vcell(real) i Icell
			//para medir Vcell llamaremos a la funcion que cree el Leva con ADCs

			Icell = calculateIcellCurrent(adcValue);

			//send data to host -> time and value
			time_counter = (point+1)*samplingPeriodMs;
			point++;
			data.point = point;
			data.timeMs = time_counter;
			data.voltage = Vcell;
			data.current = Icell;
			MASB_COMM_S_sendData(data);


		}
	}






}
