/*
 * pwm_motor.c
 *
 *  Created on: 10.12.2013
 *      Author: Raphael Dümig <duemig@in.tum.de>
 */

#include "pwm_motor.h"

#include <stdio.h>
#include <assert.h>

#define CH_BACKWARDS 0x1
#define CH_FORWARDS  0x2
#define PHASE     0x0


void init_pwm_motor(PWM_Motor *pwm_motor, alt_u32 pwm_base_address, unsigned int pwm_period) {

	pwm_motor->enable = (volatile unsigned int *) pwm_base_address;
	pwm_motor->period = (volatile unsigned int *) pwm_base_address + 1;
	pwm_motor->duty1  = (volatile unsigned int *) pwm_base_address + 2;
	pwm_motor->duty2  = (volatile unsigned int *) pwm_base_address + 3;
	pwm_motor->phase1 = (volatile unsigned int *) pwm_base_address + 4;
	pwm_motor->phase2 = (volatile unsigned int *) pwm_base_address + 5;

	pwm_motor->pwm_period = pwm_period;
	pwm_motor->current_power = 0;
}


/**
 * Apply the given settings on the PWM for this motor.
 * Method should not be used from outside this file. Control the motor using the set_power method.
 *
 * For more details about the PWM ask Hardik ;-)
 *
 * @param pwm_motor pointer to the motor structure for the PWM
 * @param phase1 phase for 1st channel of the PWM
 * @param duty1 duty for 1st channel of the PWM
 * @param phase2 phase for 2nd channel of the PWM
 * @param duty2 duty for 2nd channel of the PWM
 * @param period number of steps in one PWM cycle (defines how precise the engine can be controlled)
 * @param enable the ID of the channel you want to use (use predefined constants BACKWARDS and FORWARDS)
 */
void pwm_setting(const PWM_Motor *pwm_motor, unsigned long phase1, unsigned long duty1,
											 unsigned long phase2, unsigned long duty2,
											 unsigned long period, unsigned long enable) {

	*pwm_motor->enable = enable;
	*pwm_motor->period = period;
	*pwm_motor->phase1 = phase1;
	*pwm_motor->phase2 = phase2;
	*pwm_motor->duty1  = duty1;
	*pwm_motor->duty2  = duty2;
}


void set_power(PWM_Motor *pwm_motor, float power) {
	assert(-1 <= power && power <= 1);

	unsigned int period = pwm_motor->pwm_period;

	if(power > 0)
		pwm_setting(pwm_motor, PHASE, (int)      ( power*period ), PHASE, (int)      ( power*period ), period, CH_FORWARDS);
	else
		pwm_setting(pwm_motor, PHASE, (int) ( (-1)*power*period ), PHASE, (int) ( (-1)*power*period ), period, CH_BACKWARDS);

	pwm_motor->current_power = power;
}


float get_power(const PWM_Motor *pwm_motor) {
	return pwm_motor->current_power;
}
