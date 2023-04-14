/*
 * Motors.h
 *
 *  Created on: Feb 24, 2023
 *      Author: JPM
 */

#ifndef SOURCES_MOTORS_H_
#define SOURCES_MOTORS_H_

#include "PE_Types.h"
#include "Events.h"

// Public typedefs
typedef enum MotorDir_t {
	MotorDir_Forward,
	MotorDir_Backward,
	MotorDir_BrakeTop,
	MotorDir_BrakeBottom,
} MotorDir_t;

// Public functions
void motors_set(MotorDir_t direction, uint16_t duty);
//uint16_t motors_get_speed(void);
//MotorDir_t motors_get_dir(void);

#endif /* SOURCES_MOTORS_H_ */
