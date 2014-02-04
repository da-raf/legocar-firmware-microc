/*
 * ins.h
 *
 *  Created on: 03.12.2013
 *      Author: Raphael Dümig <duemig@in.tum.de>
 */

#ifndef INS_H_
#define INS_H_

#include <alt_types.h>

//! number of dimensions, the accelerometer is designed measure
#define GSENSOR_DIM 3


//! conversion factor from one step in the output to m/s²
const double ms2_per_digi = 0.04;

//! number of tries to read a value from the acceleration sensor before we give up
const int sensor_read_tries = 1000;

//! time in ms to wait until we try to get the next XYZ-data from the g-sensor
const int data_wait  = 1;



/**
 * Inertial Navigation System
 *
 * computes the current speed and position from acceleration data
 */
typedef struct INS {
	alt_u32 sensor_spi_base_addr;
	double  sensor_calibration[GSENSOR_DIM]; // needs VERY precise quantification
	double  acceleration[GSENSOR_DIM];
	double  speed[GSENSOR_DIM];
	double  distance[GSENSOR_DIM];
} INS;



/**
 * Create a new INS
 *
 * @param ins pointer to reserved memory
 * @param sensor_spi_base_address spi-base-address of the sensor
 *
 * @return pointer to the initialized INS structure (identical to ins parameter)
 */
INS *init_ins(INS *ins, alt_u32 sensor_spi_base_addr);


/**
 * Calibrate the INS manually.
 *
 * @param x average value for x-axis (1st axis)
 * @param y average value for y-axis (2nd axis)
 * @param z average value for z-axis (3rd axis)
 */
void calibrate_ins(INS *ins, double x, double y, double z);


/**
 * Calculate the calibration of the INS using the output of the sensor.
 * We will calculate the average of the first #values data packages from the sensor.
 *
 * @param ins the INS
 * @param values number of data packages from the sensor to use for the calibration
 *
 * @return 1 if automatic calibration was successful, 0 else
 */
int  auto_calibrate_ins(INS *ins, int values);


/**
 * Wait until the next data item from the sensor is available, or the maximum number
 * of tries is exceeded.
 *
 * @param ins the INS
 *
 * @return 1: next data package is available, 0: maximum number of tries exceeded
 */
int  wait_for_data(INS *ins);


/**
 * Update the INS with the next data package from the sensor.
 *
 * @param ins the INS
 * @param timestep time difference since the last update (to calculate updates in speed and distance)
 *
 * @result 1: success, 0: no data available or sensor read error
 */
int  update_ins(INS *ins, double timestep);


#endif /* INS_H_ */
