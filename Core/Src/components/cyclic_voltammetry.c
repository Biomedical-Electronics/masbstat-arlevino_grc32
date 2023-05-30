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

extern volatile _Bool timer;
extern MCP4725_Handle_T hdac;
extern volatile enum Estado estado;

static struct CV_Configuration_S prvCvConfiguration;

static uint32_t point = 0;
static uint32_t time_counter = 0;
uint32_t samplingPeriodMs;
int8_t sign_eStep = 1;

void CV_init(struct CV_Configuration_S cvConfiguration){

	prvCvConfiguration = cvConfiguration;
	estado = CV;
	//set the desired voltage of Vcell to eBegin using the DAC
	VCELL = calculateDacOutputVoltage(prvCvConfiguration.eBegin); // distinguish from Vcell, this one refers to DAC
	MCP4725_SetOutputVoltage(hdac, VCELL);

	//set the objective voltage to eVertex1
	vObjective = prvCvConfiguration.eVertex1;

	if (eVertex1 < eBegin) {

		sign_eStep = -1;

	}

	//close reley -> Set RELAY GPIO to 1
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET); // RELAY = 1

	//define the sampling period -> time at which the timer must trigger the ISR to toggle "timer"
	samplingPeriodMs = prvCvConfiguration.eStep*1000 / prvCvConfiguration.scanRate;

	set_up_Timer(samplingPeriodMs);//initializate timer

}



void make_CV(struct CV_configuration_S CV_config) {

	uint16_t n_cycles = 0; //cycle counter

	if (n_cycles >= prvCvConfiguration.cycles){
			estado = IDLE;

			//Open Relay
			HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);

			//Stop timer
			stop_Timer();

	}else if (timer){ //if timer is True (samplingperiodMs has passed)
			timer = FALSE; //set the variable at false again so the loop wont happen forever

			//medir Vcell(real) i Icell
			Vcell = get_Vcell();
			Icell = get_Icell();

			//enviar datos al Host
			time_counter = (point+1)*samplingPeriodMs; //cutre, preguntar si se hace asi
			point++; //se tiene que definir como uint32_t en algun lado, preguntar al albert

			data.point = point;
			data.timeMs = time_counter;
			data.voltage = Vcell;
			data.current = Icell;

			MASB_COMM_S_sendData(data);

			//assess whether I have reached vobjective

			if (VCELL == vObjective) {

				if (vObjective == prvCvConfiguration.eVertex1) {

					sign_eStep = - sign_eStep;
					vObjective = prvCvConfiguration.eVertex2;


				}else if (vObjective == prvCvConfiguration.eVertex2) {

					sign_eStep = - sign_eStep;
					vObjective = prvCvConfiguration.eBegin;
					n_cycles ++;

				}else if (n_cycles == prvCvConfiguration.cycles) {

					estado = IDLE;

					//Open Relay
					HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);

					//Stop timer
					stop_Timer();

				}else {

					vObjective = prvCvConfiguration.eVertex1;
				}

			}else{

				//assess whether the next step will be the last one (implemented for both directions with sign_eStep)
				if (sign_eStep*(VCELL + sign_eStep*prvCvConfiguration.eStep) > sign_eStep*vObjective){

					//set VCELL to vObjective
					VCELL = calculateDacOutputVoltage(vObjective);
					MCP4725_SetOutputVoltage(hdac, VCELL);


				}else {

					//update VCELL with the positive/negative step
					VCELL = VCELL + sign_eStep*prvCvConfiguration.eStep;
				}

			}


		}
	}
