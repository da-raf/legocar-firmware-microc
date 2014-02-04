/*
 * legocar.h
 *
 *  Created on: 28.01.2014
 *      Author: Raphael Dümig <duemig@in.tum.de>
 */

#ifndef LEGOCAR_H_
#define LEGOCAR_H_

#include "pwm_motor.h"

// indices of the engines responsible for rotating/aligning the wheels
// compare labels on the car:  label_value - 1 = index
#define FRONT_LEFT  0
#define FRONT_RIGHT 1
#define BACK_LEFT   2
#define BACK_RIGHT  3

/*
 * for example:
 *
 * legocar.direction[3]
 * -> the engine that controls the direction of the right wheel in the back of the car
 *
 * legocar.speed[2]
 * -> the engine that controls the speed of the right wheel in the front of the car
 */


// indices for the different driving modes
#define MOVE_DIAGONAL 0
#define MOVE_ROTATE   1
#define MOVE_CURVE    2


typedef struct LegoCar {
	PWM_Motor speed[4];
	PWM_Motor direction[4];

	int hold_direction_mode;
	float hold_direction;
	float hold_speed;
} LegoCar;



/**
 * "Create a legocar" from the addresses of the PWMs of its 8 engines.
 *
 * @param car pointer to reserved memory, big enough for the LegoCar structure
 * @param motor_base_addresses array containing the 8 base-addresses of the PWMs of the motors
 * @param pwm_period precision of the PWMs
 */
void init_legocar( LegoCar *car, alt_u32 motor_base_addresses[8], int pwm_period );


/**
 * Move all wheels of the car to the direction that is necessary for the given
 * driving pattern.
 *
 * @param car the legocar
 * @param type the driving pattern (MOVE_DIAGONAL, MOVE_ROTATE, MOVE_CURVE)
 * @param direction how far the wheels should be turned (no effect for driving pattern MOVE_ROTATE)
 */
void align_wheels( LegoCar *car, int type, float direction );


/**
 * Make the car running at the specified speed.
 *
 * @param car the legocar
 * @param type the driving pattern (MOVE_DIAGONAL, MOVE_ROTATE, MOVE_CURVE)
 * @param power power to apply on the engines (value between -1 -- full power backwards -- and 1 -- full power forwards)
 */
void set_driving_power(LegoCar *car, int movement_type, float power);


/**
 * Stop all movements of the car immediately.
 *
 * @param car the legocar
 */
void stop(LegoCar *car);


/**
 * As long as the 'control_loop' function is running, it will apply a wheel
 * stabilization procedure (to avoid drifting of the step engines, that control the
 * direction of the wheels).
 *
 * @param car the legocar
 */
void enable_wheel_stabilizer(LegoCar *car);


/**
 * Deactivate wheel stabilization.
 *
 * @param car the legocar
 */
void disable_stabilizer(LegoCar *car);


/**
 * This function is running infinitely, and applies the wheel stabilization procedure
 * by repeatedly calling realign_wheel for all the step engines that control the direction
 * of the wheels, if wheel stabilization has been activated with
 * 'enable_wheel_stabilizer'. Otherwise it will not do anything.
 * The wheel stabilizer can be switched on and off while the control loop is running.
 * The change will take effect immediately.
 *
 * @param car the legocar
 */
void control_loop(LegoCar *car);


#endif /* LEGOCAR_H_ */
