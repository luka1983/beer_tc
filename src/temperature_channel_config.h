/* temperature_channel_config.h - temperature senzor configuration
 *
 * Copyright (C) 2016 Matija Glavinic Pecotic
 *
 * You may copy, distribute and modify the software as long
 * as you track changes/dates in source files. Any modifications
 * to or software including (via compiler) GPL-licensed code must
 * also be made available under the GPL along with build & install
 * instructions.
 */

#ifndef TEMPERATURE_CHANNEL_CONFIG_H
#define TEMPERATURE_CHANNEL_CONFIG_H

#include <avr/io.h>
#include "temperature.h"

/* Sensor configuration for channel 1 */
struct temperature_channel channel_1 = {
	.sensor = temperature_sensor_ds18b20,
	.result_multiplier = DS18B20_MUL,
	.ds18b20_port = &PORTD,
	.ds18b20_direction = &DDRD,
	.ds18b20_portin = &PIND,
	.pin_mask = 1 << 6
};
#endif
