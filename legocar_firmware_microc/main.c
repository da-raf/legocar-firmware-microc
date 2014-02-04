/*
 * main.c
 *
 * Main file of the legocar project in the winter-semester 2013/14
 * at Technische Universität München.
 *
 * Author: Raphael Dümig <duemig@in.tum.de>
 */

#include <stdio.h>
#include <unistd.h>
#include <system.h>
#include <io.h>
#include <alt_types.h>

// includes of the MicroC-OS
#include "includes.h"

// structure and methods for controlling the movement of the car
#include "motor_control/legocar.h"
// reading and working with the output of an acceleration sensor
#include "acceleration_sensor/ins.h"


// priorities of the different tasks
#define STABILIZER_PRIORITY 1
#define    CONTROL_PRIORITY 2
#define ACC_SENSOR_PRIORITY 3


// size of the stacks for the different tasks
#define TASK_STACKSIZE 1024

// stacks for the tasks
OS_STK  acc_sensor_task_stk[TASK_STACKSIZE];
OS_STK  stabilizer_task_stk[TASK_STACKSIZE];
OS_STK     control_task_stk[TASK_STACKSIZE];


// our LEGO-car
LegoCar car;

// Inertial Navigation System -- built around acceleration sensor
INS ins;


// task for parsing the output of the acceleration sensor
void acc_sensor_task(void *pdata) {

	// auto calibration:
	printf("acc-sensor: calibrating acceleration sensor...\n");

	if(!auto_calibrate_ins(&ins, 1000)) {
		printf("acc-sensor: automatic calibration failed!\nShutting down sensor task...\n");
		return;
	}

	// manual calibration:
	// calibrate_ins(ins, 0.0, 0.46, -0.4);

	printf( "acc-sensor: calibration successful: %7.4f, %7.4f, %7.4f\n", ins.sensor_calibration[0],
																		 ins.sensor_calibration[1],
																		 ins.sensor_calibration[2] );

	// time to wait between two values from the sensor (in seconds)
	double timestep = 0.001;

	int i = 0;
	while(1) {

		// update the INS with new values from the sensor
		if(!update_ins(&ins, timestep)) {
			printf("acc-sensor: reading failed! Skipping...\n");
			continue;
		}

		if(i++ % 100 == 0) printf("acc-sensor: acceleration (%d): X: %6.2f,\tY: %6.2f,\tZ: %6.2f\n", i, ins.acceleration[0], ins.acceleration[1], ins.acceleration[2]);
		// printf("acc-sensor: speed (%d): X: %6.2f,\tY: %6.2f,\tZ: %6.2f\n", i, ins.speed[0], ins.speed[1], ins.speed[2]);

		// wait for one millisecond
		OSTimeDlyHMSM(0, 0, 0, timestep * 1000);
	}

}


// task for stabilizing the direction of the wheels
void stabilizer_task(void *data) {
	control_loop(&car);

	return;
}


// task for steering the car
void control_task(void *data) {

	align_wheels(&car, MOVE_DIAGONAL, 1);
	set_driving_power(&car, MOVE_DIAGONAL, 0.5);

	enable_wheel_stabilizer(&car);

	while(1) {

		printf("straight\n");
		align_wheels(&car, MOVE_DIAGONAL, 0);

		set_driving_power(&car, MOVE_DIAGONAL, 0.5);

		OSTimeDlyHMSM(0,0,5,0);

		printf("parallel\n");
		align_wheels(&car, MOVE_DIAGONAL, 0.8);

		OSTimeDlyHMSM(0,0,5,0);

		printf("circle\n");
		align_wheels(&car, MOVE_ROTATE, 1);
		// two wheels have to rotate inverted
		set_driving_power(&car, MOVE_ROTATE, 0.5);

		OSTimeDlyHMSM(0,0,5,0);

		printf("curve\n");
		align_wheels(&car, MOVE_CURVE, 0.8);
		set_driving_power(&car, MOVE_CURVE, 0.5);

		OSTimeDlyHMSM(0,0,5,0);

	}

/*
 * other example:
 *
	while(1) {
		float acceleration = 0.0005;
		float speed;

		for(speed=0; speed<1; speed += acceleration) {
			set_driving_speed(&car, speed);
			usleep(1000);
		}

		for(speed=1; speed>-1; speed -= acceleration) {
			set_driving_speed(&car, speed);
			usleep(1000);
		}

		for(speed=-1; speed<0; speed += acceleration) {
			set_driving_speed(&car, speed);
			usleep(1000);
		}
	}
*/

}



int main (void)
{

	int pwm_period = 100000;
	alt_u32 motor_base_addresses[] = { 0x80000000 | A_2_CHANNEL_PWM_0_BASE, 0x80000000 | A_2_CHANNEL_PWM_1_BASE,
									   0x80000000 | A_2_CHANNEL_PWM_2_BASE, 0x80000000 | A_2_CHANNEL_PWM_3_BASE,
	                                   0x80000000 | A_2_CHANNEL_PWM_4_BASE, 0x80000000 | A_2_CHANNEL_PWM_5_BASE,
	                                   0x80000000 | A_2_CHANNEL_PWM_6_BASE, 0x80000000 | A_2_CHANNEL_PWM_7_BASE };

	// initialize data structures for the engines so that we can control them
	init_legocar(&car, motor_base_addresses, pwm_period);

	// initialize Inertial Navigation System
	// --> the structure is used to exchange data between threads, so it has to be volatile
	init_ins(&ins, GSENSOR_SPI_BASE);


	printf("Starting system!\n");

	OSInit();

	// create the task for the wheel stabilization procedure
	OSTaskCreateExt(stabilizer_task,
		            NULL,
		            (void *) &stabilizer_task_stk[TASK_STACKSIZE-1],
		            STABILIZER_PRIORITY,
		            STABILIZER_PRIORITY,
		            stabilizer_task_stk,

		            TASK_STACKSIZE,
		            NULL,
		            0);

	// create the task for steering the car
	OSTaskCreateExt(control_task,
					NULL,
					(void *) &control_task_stk[TASK_STACKSIZE-1],
					CONTROL_PRIORITY,
					CONTROL_PRIORITY,
					control_task_stk,
					TASK_STACKSIZE,
					NULL,
					0);

/* Third task messes up the system :-(
 * When this thread is activated additionally to the other two threads,
 * the main-function will be started over and over again.
 * If you use this thread alone, there are no problems.
 * Sorry, but I cannot tell you why!
 *
 * Raphael
 *
	// create the task for parsing the output of the acceleration sensor
	OSTaskCreateExt(acc_sensor_task,
		            NULL,
		            (void *) &acc_sensor_task_stk[TASK_STACKSIZE-1],
		            ACC_SENSOR_PRIORITY,
		            ACC_SENSOR_PRIORITY,
		            acc_sensor_task_stk,
		            TASK_STACKSIZE,
		            NULL,
		            0);*/

	// start the operating system => all registered threads will be started
	OSStart();

	return 0;
}

