/*
 * INS.cpp
 *
 *  Created on: 03.12.2013
 *      Author: Raphael Dümig <duemig@in.tum.de>
 */

#include "ins.h"

// delay function from MicroC-OS is needed
#include "includes.h"

#include <unistd.h>

#include "../terasic_lib/terasic_includes.h"
#include "../terasic_lib/accelerometer_adxl345_spi.h"




INS *init_ins(INS *ins, alt_u32 sensor_spi_base_addr) {

	ins->sensor_spi_base_addr = sensor_spi_base_addr;

	ADXL345_SPI_Init( ins->sensor_spi_base_addr );

	// initialize the arrays
	int j;
	for(j=0; j<GSENSOR_DIM; j++) {
		ins->sensor_calibration[j] = 0.0;
		ins->acceleration[j]       = 0.0;
		ins->speed[j]              = 0.0;
		ins->distance[j]           = 0.0;
	}

	return ins;
}

void calibrate_ins(INS *ins, double x, double y, double z) {
	ins->sensor_calibration[0] = x;
	ins->sensor_calibration[1] = y;
	ins->sensor_calibration[2] = z;
}

int auto_calibrate_ins(INS *ins, int values) {
	// this value will be returned
	// set to 0 when we cannot read from the sensor
	int success = 1;

	// sensor output
	alt_16 cal_acceleration[GSENSOR_DIM];

	// initialize the array that is used below for calculating the average output
	// of the acceleration sensor
	double sum[GSENSOR_DIM];
	int j;
	for(j=0; j<GSENSOR_DIM; j++)
		sum[j] = 0.0;

	// read #values values from the sensor
	int i;
	for(i=0; i<values; i++) {
		// wait for next value from sensor
		if(!wait_for_data(ins)) {
			success = 0;
			return success;
		}

		// read acceleration data from the sensor
		success = ADXL345_SPI_XYZ_Read( ins->sensor_spi_base_addr, (alt_u16 *) cal_acceleration );

		if(!success) {
			printf("ERROR: cannot read from sensor!\n");
			return success;
		}

		// update sums with the latest output
		for(j=0; j<GSENSOR_DIM; j++)
			sum[j] += cal_acceleration[j];

		// make a small pause until we try to get the next data from the sensor
		OSTimeDlyHMSM(0, 0, 0, data_wait);

	}

	// calculate the average values for all the dimensions
	for(j=0; j<GSENSOR_DIM; j++)
		ins->sensor_calibration[j] = sum[j] / values * ms2_per_digi;

	// reset the speed to 0 as the system has to be standing still now
	for(j=0; j<GSENSOR_DIM; j++)
		ins->speed[j] = 0.0;

	return success;
}

int wait_for_data(INS *ins) {
	int i = 0;

	while( !ADXL345_SPI_IsDataReady( ins->sensor_spi_base_addr ) ) {
		if(i++ >= sensor_read_tries)
			return 0;

		OSTimeDlyHMSM(0, 0, 0, data_wait);
	}

	return 1;
}

int update_ins(INS *ins, double timestep) {

	// check the sensor calibration for Z-axis: if this is 0, the ins has probably not been calibrated
	// => g-force
	if(ins->sensor_calibration[2] == 0)
		printf("WARNING: g-sensor has probably not been calibrated!\n");

	if(!wait_for_data(ins))
		return 0;

	bool success;
	alt_16 acc[GSENSOR_DIM];

	success = ADXL345_SPI_XYZ_Read(ins->sensor_spi_base_addr, (alt_u16 *) acc);
	if(!success) {
		printf("ERROR: reading from sensor failed!\n");
		return 0;
	}

	// update the state of the ins
	int j;
	for(j=0; j<GSENSOR_DIM; j++) {
		ins->acceleration[j] = ((int) acc[j]) * ms2_per_digi - ins->sensor_calibration[j];
		ins->speed[j]       += ins->acceleration[j] * timestep;
		ins->distance[j]    += ins->speed[j]        * timestep;
	}

	return 1;
}
