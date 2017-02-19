/* temperature.c
 *
 * Copyright (C) 2016 Matija Glavinic Pecotic
 *
 * You may copy, distribute and modify the software as long
 * as you track changes/dates in source files. Any modifications
 * to or software including (via compiler) GPL-licensed code must
 * also be made available under the GPL along with build & install
 * instructions.
 */
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include "temperature.h"
#include "temperature_channel_config.h"

#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))

static int16_t temperature_read_ds18b20(struct temperature_channel *tc)
{
	int16_t err = 0;

	err = ds18b20convert(tc->port, tc->direction, tc->portin, tc->pin_mask, NULL);
	if (err != 0)
		return err;

	/* see ds18b20 ds, temp conversion in 12 bit resolution
	 * can last up to 750 ms */
	_delay_ms(1000);

	err = ds18b20read(tc->port, tc->direction, tc->portin, tc->pin_mask, NULL, &(tc->temp_raw));
	if (err != 0)
		return err;

	return err;
}

static int16_t temperature_read_ds18b20_async(struct temperature_channel *tc)
{
	int16_t err = 0;

	// get last reading
	err = ds18b20read(tc->port, tc->direction, tc->portin, tc->pin_mask, NULL, &(tc->temp_raw));
	if (err != 0)
		return err;

	// initiate new read
	err = ds18b20convert(tc->port, tc->direction, tc->portin, tc->pin_mask, NULL);
	if (err != 0)
		return err;

	return err;
}

int16_t temperature_read(struct temperature_channel *tc, uint8_t async)
{
	int16_t err = 0;

	switch (tc->sensor) {
		case temperature_sensor_ds18b20:
			if (async)
				err = temperature_read_ds18b20_async(tc);
			else
				err = temperature_read_ds18b20(tc);
			break;
		default:
			err = TEMPERATURE_UNKNOWN_SENSOR_TYPE;
	}

	return err;
}

struct temperature_channel *temperature_get_channel_by_name(const char *name)
{
	int16_t i;

	for(i = 0;i < ARRAYSIZE(channels);i++) {
		if (strcmp(channels[i].name, name) == 0)
			return &channels[i];
	}

	return NULL;
}

void temperature_get_all_channel_names(int16_t *size, char *names[])
{
	for(*size = 0;*size < ARRAYSIZE(channels);(*size)++) {
		names[*size] = channels[*size].name;
	}
}

int16_t temperature_get_nr_channels(void)
{
	return ARRAYSIZE(channels);
}
