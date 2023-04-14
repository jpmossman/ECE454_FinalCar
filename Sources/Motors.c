/*
 * Motors.c
 *
 *  Created on: Feb 24, 2023
 *      Author: JPM
 */

#include "Motors.h"

// Private variables
MotorDir_t CurrentDirection = MotorDir_Forward;
uint16_t CurrentSpeed = 0;
#define MIN_DUTY 0xFFFF

// Public function definitions
void motors_set(MotorDir_t dir, uint16_t speed) {
	CurrentDirection = dir;
	CurrentSpeed = speed;

	switch (dir) {
	case MotorDir_Forward:
		PWM_BA_SetRatio16(MIN_DUTY);
		PWM_BB_SetRatio16(MIN_DUTY);
		PWM_FA_SetRatio16(MIN_DUTY - speed);
		PWM_FB_SetRatio16(MIN_DUTY - speed);
		break;

	case MotorDir_Backward:
		PWM_FA_SetRatio16(MIN_DUTY);
		PWM_FB_SetRatio16(MIN_DUTY);
		PWM_BA_SetRatio16(MIN_DUTY - speed);
		PWM_BB_SetRatio16(MIN_DUTY - speed);
		break;

	case MotorDir_BrakeTop:
		PWM_BA_SetRatio16(MIN_DUTY);
		PWM_FB_SetRatio16(MIN_DUTY);
		PWM_FA_SetRatio16(MIN_DUTY - speed);
		PWM_BB_SetRatio16(MIN_DUTY - speed);
		break;

	case MotorDir_BrakeBottom:
		PWM_FA_SetRatio16(MIN_DUTY);
		PWM_BB_SetRatio16(MIN_DUTY);
		PWM_BA_SetRatio16(MIN_DUTY - speed);
		PWM_FB_SetRatio16(MIN_DUTY - speed);
		break;

	default: // Panic
		PWM_FA_SetRatio16(MIN_DUTY);
		PWM_BB_SetRatio16(MIN_DUTY);
		PWM_BA_SetRatio16(MIN_DUTY - speed);
		PWM_FB_SetRatio16(MIN_DUTY - speed);
		break;
	}
}

//uint16_t motors_get_speed(void) {
//	return CurrentSpeed;
//}
//
//MotorDir_t motors_get_dir(void) {
//	return CurrentDirection;
//}


// Private function definitions


