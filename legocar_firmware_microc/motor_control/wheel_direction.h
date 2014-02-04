/*
 * wheel_direction.h
 *
 *  Created on: 27.01.2014
 *      Author: raphael
 */

#include "pwm_motor.h"

#ifndef WHEEL_DIRECTION_H_
#define WHEEL_DIRECTION_H_


// this constant defines how far the wheels can be turned
// 1.0: full range of the step motor
// 0.25: on quarter of the range of the step motor
#define TURNING_INTERVAL 0.4


/**
 * Align the wheel straight.
 *
 * @param direction_motor pointer to the PWM for the step motor for this wheel
 */
int init_direction(PWM_Motor *direction_motor);

/**
 * Move the wheel to the specified direction.
 *
 * @param direction_motor pointer to the PWM for the step motor that controls the direction
 * @param direction value between -1 and 1: 1 => maximum turn anti-clockwise; -1 => maximum turn clockwise
 */
int set_direction(PWM_Motor *direction_motor, float direction);

/**
 * Get the direction the wheel SHOULD currently have.
 * This is not checked in hardware, so if the wheel has been forced to a different position we will not
 * notice that here!
 *
 * @param direction_motor the address of the PWM for this step motor
 * @result value between -1 and 1: 1 => maximum turn anti-clockwise; -1 => maximum turn clockwise
 */
float get_direction(PWM_Motor *direction_motor);

/**
 * Correct the direction of the wheel in case it has been moved out of the correct position.
 * This function is able to realign the wheel if the step-motor has been forced to a different position.
 * If the motor has lost mechanical contact from the wheel (gears slipping) then we have no chance
 * to correct this.
 *
 * @param direction_motor the address of the PWM for this step motor
 */
void realign_wheel(PWM_Motor *direction_motor);

#endif /* WHEEL_DIRECTION_H_ */
