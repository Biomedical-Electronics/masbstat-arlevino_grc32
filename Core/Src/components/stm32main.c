/*
 * stm32main.c
 *
 *  Created on: 23 may. 2023
 *      Author: willy
 */

#include "components/stm32main.h"
#include "components/masb_comm_s.h"
#include "components/PMU.h"

struct CV_Configuration_S cvConfiguration;
struct CA_Configuration_S caConfiguration;

volatile enum Estado{IDLE = 0, CV, CA}estado;

MCP4725_Handle_T hdac = NULL;

void setup(struct Handles_S *handles) {
	PMU_init();

	I2C_init(&hi2c1);

	AD5280_Handle_T hpot = NULL;

	hpot = AD5280_Init();

	// We configure its slave I2C address, its total resistance (there are
	// different models; this one has 50kohms) and we indicate which function we want it to
	// be in charge of writing through the I2C. We will use the function
	// I2C_Write from the i2c_lib library.
	AD5280_ConfigSlaveAddress(hpot, 0x2C);
	AD5280_ConfigNominalResistorValue(hpot, 50e3f);
	AD5280_ConfigWriteFunction(hpot, I2C_write);

	// We set the resistance to 50 kOhm.
	AD5280_SetWBResistance(hpot, 50e3f);

	// Declare this variable as a global variable in order to be able to
	// access it from different archives

	hdac = MCP4725_Init();
	MCP4725_ConfigSlaveAddress(hdac, 0x66); // SLAVE ADDRESS
	MCP4725_ConfigVoltageReference(hdac, 4.0f); // REFERENCE VOLTAGE
	MCP4725_ConfigWriteFunction(hdac, I2C_write); // WRITING FUNCTION (I2C_lib library)

    MASB_COMM_S_setUart(handles->huart);
    MASB_COMM_S_waitForMessage();
}


void loop(void) {
    if (MASB_COMM_S_dataReceived()) { // If a message has been received...

 		switch(MASB_COMM_S_command()) { // We look at what command we have received

 			case START_CV_MEAS: // If we have received START_CV_MEAS

                 // We read the configuration that has been sent to us in the message and
                 // we store it in the variable cvConfiguration
				cvConfiguration = MASB_COMM_S_getCvConfiguration();

 				CV_init(cvConfiguration);

 				break;

 			case START_CA_MEAS: // If we have received START_CA_MEAS

                // We read the configuration that has been sent to us in the message and
                 // we store it in the variable caConfiguration
 				caConfiguration = MASB_COMM_S_getCaConfiguration();

 				CA_init(caConfiguration);


 			 	break;

			case STOP_MEAS: // If we have received STOP_MEAS

 				//stop the timer
 				stop_Timer();
 				//Change enum variable to IDLE
 				estado = IDLE;
 				//Open Relay
 				HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);

 				break;

 			default: // In the event that a command is sent which does not exist

 				break;

 		}

       // Once the commands have been processed, we wait for the following
 		MASB_COMM_S_waitForMessage();

 	}

    switch(estado){

    	case CV: //If we are measuring CV, read one point and send it

    		make_CV();

    		break;

    	case CA: //If we are measuring CA, read one point and send it

    		make_CA();

    		break;

    	case IDLE: // Do not do anything

    		break;
    }

}
