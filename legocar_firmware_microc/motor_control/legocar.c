/*
 * legocar.c
 *
 *  Created on: 28.01.2014
 *      Author: Raphael Dümig <duemig@in.tum.de>
 */

#include "legocar.h"
#include "wheel_direction.h"

// imports from MicroC-OS: delay function for direction stabilization
#include "includes.h"

#include <stdio.h>
#include <unistd.h>


void init_legocar(LegoCar *car, alt_u32 motor_base_addresses[8], int pwm_period) {
	int w;

	for(w=0; w<8; w++) {
		if( w < 4 )
			init_pwm_motor(&(car->speed[w]), motor_base_addresses[w], pwm_period);
		else
			init_pwm_motor(&(car->direction[w-4]), motor_base_addresses[w], pwm_period);
	}

}

void align_wheels(LegoCar *car, int type, float direction) {

	int w;

	for(w=0; w<4; w++) {
		switch(type) {
		case MOVE_DIAGONAL:
			set_direction(&car->direction[w], direction);
			break;

		case MOVE_ROTATE:
			if(w == 0 || w == 3)
				set_direction(&car->direction[w], -1);
			else
				set_direction(&car->direction[w], +1);
			break;

		case MOVE_CURVE:
			if(w < 2)
				set_direction(&car->direction[w], direction);
			else
				set_direction(&car->direction[w], -direction);
			break;

		default:
			printf("!!!Invalid wheel alignment type!!!\n");
			break;
		}

		printf("wheel %d: direction %f\n", w+1, get_direction(&car->direction[w]));
	}

	return;
}

void set_driving_power(LegoCar *car, int type, float power) {

	int w;

	// set the correct speed on every wheel
	for(w=0; w<4; w++)
		switch(type) {

		case MOVE_DIAGONAL:
		case MOVE_CURVE:
			set_power(&(car->speed[w]), power);
			break;

		case MOVE_ROTATE:
			set_power(&(car->speed[w]), (w % 2 == 0) ? -power : power);
			break;
		}

}

void stop(LegoCar *car) {
	set_driving_power(car, MOVE_CURVE, 0);
}

void enable_wheel_stabilizer(LegoCar *car) {
	car->hold_direction_mode = 1;
}

void disable_wheel_stabilizer(LegoCar *car) {
	car->hold_direction_mode = 0;
}

void control_loop(LegoCar *car) {

	while(1) {

		// if car.hold_direction_mode is set then realign the wheels
		if( car->hold_direction_mode ) {

			int w = 0;
			for(w=0; w<4; w++)
				realign_wheel(&car->direction[w]);
		}

		// wait for 10 milliseconds
		OSTimeDlyHMSM(0, 0, 0, 10);
	}

}

