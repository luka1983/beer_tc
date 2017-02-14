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
	/* Type of the sensor */
	sensor_t sensor;

	/* Return value multiplier */
	uint16_t result_multiplier;

	/* ds18b20 specific info */
	/* PORT to which ds18b20 is connected */
	volatile uint8_t *ds18b20_port;
	/* Direction register of the port */
	volatile uint8_t *ds18b20_direction;
	/* PORTIN register to which ds18b20 is connected */
	volatile uint8_t *ds18b20_portin;
	/* mask of the pin to which ds18b20 is connected */
	uint8_t pin_mask;
};

/**
 * temperature_read - Read temperature from channel
 *
 * Function reads temperature from predefined channel. Depending
 * on the sensor, function can block for some amount of time. Here
 * are known durations for various sensors:
 *
 * ds18b20 - 1000 ms
 * 
 * Parameters:
 *
 * tc [IN] - temperature channel config
 * temperature [OUT] - temperature which may be multiplied
 *		       with multiplier. Check tc.result_multiplier
 *
 * Returns:
 * Function returns 0/TEMPERATURE_OK in case all went fine, or
 * error code defined in temperature.h
 *
 * Usage:
 * First, define channel data, this should be put in e.g.
 * temperature_channel_config.h:
 *
 * struct temperature_channel channel_1 = {
 *	.sensor = temperature_sensor_ds18b20,
 *	.result_multiplier = DS18B20_MUL,
 *	.ds18b20_port = &PORTD,
 *	.ds18b20_direction = &DDRD,
 *	.ds18b20_portin = &PIND,
 *	.pin_mask = 1 << 6
 * };
 *
 * Here it is assumed ds18b20 is used. Next, use defined channel in
 * reading the temperature, e.g.
 *
 * Defines:
 * #include <util/delay.h>
 * #include <stdlib.h>
 * #include "temperature.h"
 * #include "temperature_channel_config.h"
 *
 * Code:
 * int temp_int = 0, err = 0;
 * double temp_double = 0;
 * char string_temperature[32];
 *
 * err = temperature_read(&channel_1, &temp_int);
 * if (err != 0) {
 *	sprints("Error (%d) while temperature_read", err);
 *	return;
 * }
 *
 * temp_double = (double)temp_int/(double)channel_1.result_multiplier;
 * dtostrf(temp_double, 2, 4, string_temperature);
 * sprints("temperature_demo: temp (int * %d) = %d, temp (double) = %s",
 *		channel_1.result_multiplier, temp_int, string_temperature);
 * */
extern int16_t temperature_read(struct temperature_channel *tc, int16_t *temperature);
#endif
