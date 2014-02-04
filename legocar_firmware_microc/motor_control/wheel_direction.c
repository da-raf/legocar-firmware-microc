/*
 * wheel_direction.c
 *
 * functions to control the direction of the wheels using LEGO-step-engines
 *
 *  Created on: 27.01.2014
 *      Author: Raphael Dümig <duemig@in.tum.de>
 */

#include "wheel_direction.h"

// functions from MicroC-OS: needed for sleeping
#include "includes.h"

#include "pwm_motor.h"



int init_direction(PWM_Motor *direction_motor) {
	// align wheel straight
	set_direction(direction_motor, 0.f);

	return 1;
}


void set_direction(PWM_Motor *direction_motor, float direction) {
	// apply the necessary power to the step-motor
	set_power(direction_motor, direction * TURNING_INTERVAL);
}


float get_direction(PWM_Motor *direction_motor) {
	return get_power(direction_motor) / TURNING_INTERVAL;
}


void realign_wheel(PWM_Motor *direction_motor) {
	// Shake the wheel a little, so that the motor thinks that the wheel is in
	// the wrong position and applies force to realign it.
	// This way the wheel will stay stable in position.

	// save the correct direction, as we will move the wheel to a different one
	float hold_direction = get_direction(direction_motor);

	// try to move the wheel to a completely different position for a very short
	// period of time, that is actually much too small for moving there
	if(hold_direction <= 0)
		set_direction(direction_motor, hold_direction + 1);
	else
		set_direction(direction_motor, hold_direction - 1);

	// wait for one millisecond
	OSTimeDlyHMSM(0, 0, 0, 1);

	// now realign the wheel
	set_direction(direction_motor, hold_direction);
}
