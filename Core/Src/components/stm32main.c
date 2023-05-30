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
struct Data_S data;

volatile enum Estado{IDLE = 0, CV, CA}estado;

MCP4725_Handle_T hdac = NULL;

void setup(struct Handles_S *handles) {
	PMU_init();

	I2C_init(&hi2c1);

	AD5280_Handle_T hpot = NULL;

	hpot = AD5280_Init();

	// Configuramos su direccion I2C de esclavo, su resistencia total (hay
	// diferentes modelos; este tiene 50kohms) e indicamos que funcion queremos que
	// se encargue de la escritura a traves del I2C. Utilizaremos la funcion
	// I2C_Write de la libreria i2c_lib.
	AD5280_ConfigSlaveAddress(hpot, 0x2C);
	AD5280_ConfigNominalResistorValue(hpot, 50e3f);
	AD5280_ConfigWriteFunction(hpot, I2C_write);

	// Fijamos la resistencia de 50 kohms.
	AD5280_SetWBResistance(hpot, 50e3f);

	// Declarar esta variable como global para poder
	// acceder a ella desde diferentes archivos

	hdac = MCP4725_Init();
	MCP4725_ConfigSlaveAddress(hdac, 0x66); // DIRECCION DEL ESCLAVO
	MCP4725_ConfigVoltageReference(hdac, 4.0f); // TENSION DE REFERENCIA
	MCP4725_ConfigWriteFunction(hdac, I2C_write); // FUNCION DE ESCRITURA (libreria I2C_lib)

    MASB_COMM_S_setUart(handles->huart);
    MASB_COMM_S_waitForMessage();
}


void loop(void) {
    if (MASB_COMM_S_dataReceived()) { // Si se ha recibido un mensaje...

 		switch(MASB_COMM_S_command()) { // Miramos que comando hemos recibido

 			case START_CV_MEAS: // Si hemos recibido START_CV_MEAS

                 // Leemos la configuracion que se nos ha enviado en el mensaje y
                 // la guardamos en la variable cvConfiguration
				cvConfiguration = MASB_COMM_S_getCvConfiguration();

 				CV_init(cvConfiguration);

 				__NOP(); // Esta instruccion no hace nada y solo sirve para poder anadir un breakpoint

 				// Aqui iria todo el codigo de gestion de la medicion que hareis en el proyecto
                // si no quereis implementar el comando de stop.

 				break;

 			case START_CA_MEAS: // Si hemos recibido START_CA_MEAS

 				// Leemos la configuracion que se nos ha enviado en el mensaje y
 				// la guardamos en la variable caConfiguration
 				caConfiguration = MASB_COMM_S_getCaConfiguration();

 				CA_init(caConfiguration);

 			 	__NOP(); // Esta instruccion no hace nada y solo sirve para poder anadir un breakpoint


 			 	break;

			case STOP_MEAS: // Si hemos recibido STOP_MEAS

 				__NOP(); // Esta instruccion no hace nada y solo sirve para poder anadir un breakpoint

 				// Aqui iria el codigo para tener la medicion si implementais el comando stop.

 				//stop the timer
 				stop_Timer();
 				//Change enum variable to IDLE
 				estado = IDLE;
 				//Open Relay
 				HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);

 				break;

 			default: // En el caso que se envia un comando que no exista

 				break;

 		}

       // Una vez procesado los comando, esperamos el siguiente
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
