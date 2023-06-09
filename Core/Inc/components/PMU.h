/*
 * PMU.h
 *
 *  Created on: 16 may. 2023
 *      Author: Leva Álvaro
 *      Definiremos las funciones de control de la PMU:
 *      - PMU_init
 *      - PMU_end
 */

#ifndef INC_COMPONENTS_PMU_H_
#define INC_COMPONENTS_PMU_H_

// Incluimos este header de las HAL para poder usar algunas
// funcionalidades de las HAL.
#include "stm32f4xx_hal.h"

// Define the function prototipes to enable them in the source codes that make the include

void PMU_init(void);
void PMU_end(void);


#endif /* INC_COMPONENTS_PMU_H_ */
