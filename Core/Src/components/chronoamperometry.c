/*
 * chronoamperometry.c
 *
 *  Created on: May 16, 2023
 *      Author: willy
 */

#include "components/stm32main.h"
#include "components/masb_comm_s.h"
#include "components/formulas.h"
#include "components/mcp4725_driver.h"
#include "components/chronoamperometry.h"

volatile _Bool timer = FALSE;
//extern volatile _Bool timer ,en archivo timer
extern MCP4725_Handle_T hdac;
extern volatile _Bool stop;

void make_CA(struct CA_Configuration_S caConfiguration){

	uint32_t point = 0;
	uint32_t time_counter = 0;
	// extract the 3 variables from the input structure
	eDC = caConfiguration.eDC;
	samplingPeriodMs = caConfiguration.samplingPeriodMs;
	measurementTimeMs = caConfiguration.measurementTime*1000;

	// set Vcell to eDC
	MCP4725_SetOutputVoltage(hdac, calculateDacOutputVoltage(eDC));

	//Close Relay
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);

	//set the time at which the timer will execute the ISR
	__HAL_TIM_SET_AUTORELOAD(&htim2, samplingPeriodMs);

	//Funcion ISR del timer del archivo del Leva que cambia una variable a True
	//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
	while (time_counter < measurementTimeMs){
		if (stop) { //if stop is True (measuring must stop)
			break
		}
		if (timer){ //if timer is True (samplingperiodMs has passed)
			timer = FALSE; //set the variable at false again so the loop wont happen forever

			//medir Vcell(real) i Icell
			//para medir Vcell llamaremos a la funcion que cree el Leva con ADCs

			//Icell. No se que input hay que poner en la funcion
			Icell = calculateIcellCurrent(adcValue);

			//enviar datos al Host
			time_counter = (point+1)*samplingPeriodMs; //cutre, preguntar si se hace asi
			point++; //se tiene que definir como uint32_t en algun lado, preguntar al albert

			data.point = point;
			data.timeMs = time_counter;
			data.voltage = Vcell;
			data.current = Icell;

			MASB_COMM_S_sendData(data);
		}
	}

}
