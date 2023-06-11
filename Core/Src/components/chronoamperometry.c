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
#include "components/timer.h"
#include "components/adc.h"
#include "main.h"

extern volatile _Bool timer;
extern MCP4725_Handle_T hdac;
extern volatile _Bool stop;
extern volatile enum Estado{IDLE, CV, CA} estado;

uint32_t point_ca = 0;
static uint32_t time_counter = 0;

static struct CA_Configuration_S prvCaConfiguration;
extern struct Data_S data;

double Vcell_ca;
double Icell_ca;

void CA_init(struct CA_Configuration_S caConfiguration){

	prvCaConfiguration = caConfiguration; //obtain the received variables
	estado = CA; //change measuring state to CA

	set_up_Timer(prvCaConfiguration.samplingPeriodMs); //initialise timer

	// set Vcell to eDC
	MCP4725_SetOutputVoltage(hdac, calculateDacOutputVoltage(prvCaConfiguration.eDC));

	//Close Relay
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
}

void make_CA(void){


	if (time_counter >= prvCaConfiguration.measurementTime*1000){
		estado = IDLE;

		//Open Relay
		HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);

		//Stop timer
		stop_Timer();

	}else if (timer){ //if timer is True (samplingperiodMs has passed)

		timer = FALSE; //set the variable at false again so the loop wont happen forever

		//medir Vcell(real) i Icell
		Vcell_ca = get_Vcell();
		Icell_ca = get_Icell();

		//enviar datos al Host
		point_ca++;
		time_counter = (point_ca)*prvCaConfiguration.samplingPeriodMs;

		if (point_ca == 2){
			__NOP();
		}
		data.point = point_ca;
		data.timeMs = time_counter;
		data.voltage = Vcell_ca;
		data.current = Icell_ca;

		MASB_COMM_S_sendData(data);
	}

}
