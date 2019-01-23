/* temperature.h
 *
 * Copyright (C) 2016 Matija Glavinic Pecotic
 *
 * You may copy, distribute and modify the software as long
 * as you track changes/dates in source files. Any modifications
 * to or software including (via compiler) GPL-licensed code must
 * also be made available under the GPL along with build & install
 * instructions.
 */

#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "ds18b20.h"

#define TEMPERATURE_OK		0
#define TEMPARATURE_ERROR_COMM	1
#define TEMPERATURE_ERROR_CRC	2
#define TEMPERATURE_ERROR_PULL	3
#define TEMPERATURE_ERROR_OTHER 4
#define TEMPERATURE_UNKNOWN_SENSOR_TYPE 5

#define TEMPERATURE_MAX_NAME_SIZE 12

/* Types of sensors */
typedef enum {
	/* ds18b20 */
	temperature_sensor_ds18b20,
	/* end of list, this value must be last */
	temperature_sensor_max
} sensor_t;

/* Channel descriptor
 * This structure populates info needed to read
 * temperature sensor
 * */
struct temperature_channel {
	/* Name of the channel */
	char name[TEMPERATURE_MAX_NAME_SIZE];

	/* Raw temperature reading */
	int16_t temp_raw;

	/* Temperature reading in degrees C */
	int32_t temp_c;

	/* Type of the sensor */
	sensor_t sensor;

	/* Return value multiplier */
	uint16_t result_multiplier;

	/* ds18b20 specific info */
	/* PORT to which ds18b20 is connected */
	volatile uint8_t *port;
	/* Direction register of the port */
	volatile uint8_t *direction;
	/* PORTIN register to which ds18b20 is connected */
	volatile uint8_t *portin;
	/* mask of the pin to which ds18b20 is connected */
	uint8_t pin_mask;
};

/**
 * Function gets temperature structure by its name
 * */
struct temperature_channel *temperature_get_channel_by_name(const char *name);

/**
 * Function gets names of all supported channels
 *
 * Function returns array of strings with channel names
 * which can further be used in e.g. temperature_get_channel_by_name
 *
 * Parameters:
 *
 * size [OUT] - number of names, number of channels
 * names [OUT] - arrays of names
 * */
void temperature_get_all_channel_names(int16_t *size, char *names[]);

/**
 * Function gets number of channels
 *
 * Returns number of channels. This number may be used
 * to allocate array for temperature_get_all_channel_names
 *
 * */
int16_t temperature_get_nr_channels(void);

/**
 * temperature_read - Read temperature from channel
 *
 * Function reads temperature from predefined channel. If non async
 * version is used, depending on the sensor, the function can block
 * for some amount of time.The known durations for sensor types are:
 *
 * ds18b20 - 1000 ms
 *
 * Parameters:
 *
 * tc [IN]    - temperature channel config
 * async [IN] - async temperature read. If used then the async read
 *              is performed. In terms of ds18b20, this means that
 *              last value in sensor is taken. Temperature read works
 *              in a way that read towards sensor is initiated, then
 *              up to 750ms can pass until the result is ready. In case
 *              of async read, command to sensor will be initiated, but
 *              its completion will not be waited for, but data which
 *              was last ready will be read which in the end means
 *              temperature from last read is fetched.
 *
 * Returns:
 * Function returns 0/TEMPERATURE_OK in case all went fine, or
 * error code defined in temperature.h
 * */
int16_t temperature_read(struct temperature_channel *tc, uint8_t async);
#endif
