/* demo.h - Various usage demos
 *
 * Copyright (C) 2016 Matija Glavinic Pecotic
 *
 * You may copy, distribute and modify the software as long
 * as you track changes/dates in source files. Any modifications
 * to or software including (via compiler) GPL-licensed code must
 * also be made available under the GPL along with build & install
 * instructions.
 */

#ifndef DEMO_H
#define DEMO_H

#include <util/delay.h>
#include <stdlib.h>
#include "temperature.h"

/**
 * Temperature read example
 *
 * Function demonstrates reading temperature and going over
 * all channels. Instead of going over all channels, one could
 * immediately supply known name to function:
 * temperature_get_channle_by_name
 */
void temperature_demo(void)
{
	int temp_int = 0, err = 0, channel_no, i;
	double temp_double = 0;
	char string_temperature[12];
	char *names[10];
	struct temperature_channel **tc;

	temperature_get_all_channel_names(&channel_no, names);

	tc = (struct temperature_channel **) malloc (sizeof(struct temperature_channel*) * channel_no);

	for(i = 0; i < channel_no; i++) {
		tc[i] = temperature_get_channel_by_name(names[i]);
		if (tc[i] == NULL) {
			sprints("Could not find channel <%s>", names[i]);
		}
	}

	for(i = 0; i < channel_no; i++) {
		err = temperature_read(tc[i], &temp_int);
		if (err != 0) {
			sprints("channel %s: error (%d) while temperature_read", tc[i]->name, err);
			continue;
		}
		temp_double = (double)temp_int/(double)tc[i]->result_multiplier;

		dtostrf(temp_double, 2, 4, string_temperature);
		sprints("channel %s: temperature: %s C", tc[i]->name, string_temperature);
	}
	free(tc);
}

#endif
