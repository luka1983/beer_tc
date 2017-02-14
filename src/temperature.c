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
#include "temperature.h"

static int temperature_read_ds18b20(volatile uint8_t *ds18b20_port,
		  volatile uint8_t *ds18b20_direction,
		  volatile uint8_t *ds18b20_portin,
		  uint8_t pin_mask, int16_t *temp)
{
	int temp_int = 0, err = 0;

	err = ds18b20convert(ds18b20_port, ds18b20_direction,
			     ds18b20_portin, pin_mask, NULL);
	if (err != 0)
		return err;

	/* see ds18b20 ds, temp conversion in 12 bit resolution
	 * can last up to 750 ms */
	_delay_ms(1000);

	err = ds18b20read(ds18b20_port, ds18b20_direction,
			  ds18b20_portin, pin_mask, NULL, &temp_int);
	if (err != 0)
		return err;

	*temp = temp_int;

	return err;
}

int16_t temperature_read(struct temperature_channel *tc, int16_t *temperature)
{
	int16_t err = 0, temp = 0;
	*temperature = temp;

	switch (tc->sensor) {
	case temperature_sensor_ds18b20:
		err = temperature_read_ds18b20(tc->ds18b20_port,
					       tc->ds18b20_direction,
					       tc->ds18b20_portin,
					       tc->pin_mask,
					       &temp);
		*temperature = temp;
		break;
	default:
		err = TEMPERATURE_UNKNOWN_SENSOR_TYPE;
	}

	return err;
}
