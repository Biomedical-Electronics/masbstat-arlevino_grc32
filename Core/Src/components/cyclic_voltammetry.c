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
#include "components/timer.h"
#include "components/adc.h"

extern volatile _Bool timer;
extern MCP4725_Handle_T hdac;
extern volatile enum Estado{IDLE, CV, CA} estado;

static struct CV_Configuration_S prvCvConfiguration;
struct Data_S data;

static uint32_t point = 0;
static uint32_t time_counter = 0;
uint32_t samplingPeriodMs;
int8_t sign_eStep = 1;

float VCELL_cv;
double vObjective;

double Vcell_real_cv;
double Icell_cv;
double theor_v;

uint16_t n_cycles;

struct Handles_S myHandles;

void CV_init(struct CV_Configuration_S cvConfiguration){

	n_cycles = 1; //init cycle counter
	prvCvConfiguration = cvConfiguration;
	estado = CV;
	//set the desired voltage of Vcell to eBegin using the DAC
	theor_v = prvCvConfiguration.eBegin;
	VCELL_cv = calculateDacOutputVoltage(theor_v); // distinguish from Vcell_real, this one refers to DAC
	MCP4725_SetOutputVoltage(hdac, VCELL_cv);

	//set the objective voltage to eVertex1
	vObjective = prvCvConfiguration.eVertex1;

	if (prvCvConfiguration.eVertex1 < prvCvConfiguration.eBegin) {

		sign_eStep = -1;

	}

	//close reley -> Set RELAY GPIO to 1
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET); // RELAY = 1

	//define the sampling period -> time at which the timer must trigger the ISR to toggle "timer"
	samplingPeriodMs = prvCvConfiguration.eStep*1000 / prvCvConfiguration.scanRate;

	set_up_Timer(samplingPeriodMs);//initializate timer

}



void make_CV(void) {

	if (timer){ //if timer is True (samplingperiodMs has passed)
			timer = FALSE; //set the variable at false again so the loop wont happen forever

			//medir Vcell(real) i Icell
			Vcell_real_cv = get_Vcell();
			Icell_cv = get_Icell();

			//enviar datos al Host
			point++;
			time_counter = (point)*samplingPeriodMs;

			data.point = point;
			data.timeMs = time_counter;
			data.voltage = Vcell_real_cv;
			data.current = Icell_cv;

			MASB_COMM_S_sendData(data);

			//assess whether I have reached vobjective

			if (abs(theor_v - vObjective)< (10^(-5))) {

				if (abs(vObjective - prvCvConfiguration.eVertex1) < (10^(-5))) {

					sign_eStep = - sign_eStep;
					vObjective = prvCvConfiguration.eVertex2;


				}else if (abs(vObjective - prvCvConfiguration.eVertex2)< (10^(-5))) {

					sign_eStep = - sign_eStep;
					vObjective = prvCvConfiguration.eBegin;

				}else if (abs(n_cycles - prvCvConfiguration.cycles)< (10^(-5))) {

					estado = IDLE;

					//Open Relay
					HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);

					//Stop timer
					stop_Timer();

				}else {
					n_cycles ++;
					vObjective = prvCvConfiguration.eVertex1;
				}

			}else{

				//assess whether the next step will be the last one (implemented for both directions with sign_eStep)
				if (sign_eStep*(theor_v + sign_eStep*prvCvConfiguration.eStep) > sign_eStep*vObjective){

					//set VCELL to vObjective
					VCELL_cv = calculateDacOutputVoltage(vObjective);
					MCP4725_SetOutputVoltage(hdac, VCELL_cv);


				}else {

					//update VCELL with the positive/negative step
					theor_v = theor_v + sign_eStep*prvCvConfiguration.eStep;
					VCELL_cv = calculateDacOutputVoltage(theor_v);
					MCP4725_SetOutputVoltage(hdac, VCELL_cv);

				}

			}


		}
	}
