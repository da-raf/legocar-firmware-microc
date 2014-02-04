/*
 * pwm_motor.h
 *
 *  Created on: 10.12.2013
 *      Author: raphael
 */

#ifndef PWM_MOTOR_H_
#define PWM_MOTOR_H_

#include <alt_types.h>

typedef struct PWM_Motor {
	// bitwise encoded direction: 0x2 => forwards, 0x1 => backwards
	volatile unsigned int *enable;
	volatile unsigned int *period;
	volatile unsigned int *phase1;
	volatile unsigned int *phase2;
	volatile unsigned int *duty1;
	volatile unsigned int *duty2;

	unsigned int pwm_period;
	float current_power;
} PWM_Motor;


/**
 * Initialize the PWM_Motor structure for a motor, so that the structure is able to control the motor
 * correctly.
 */
void init_pwm_motor(PWM_Motor *pwm_motor, alt_u32 pwm_base_address, unsigned int period);


/**
 * Make the motor move at a specified speed.
 *
 * @param pwm_motor PWM_Motor structure for the motor
 * @param power value between -1 and 1: 1.0 => full power forwards; -1.0 => full power backwards; 0.0 => no power
 */
void set_power(PWM_Motor *pwm_motor, float speed);


/**
 * Get the power that is currently applied to this motor.
 *
 * @param pwm_motor the motor of that you want to know the currently applied power (between -1 and 1 - see 'set_power')
 */
float get_power(const PWM_Motor *pwm_motor);

#endif /* PWM_MOTOR_H_ */
