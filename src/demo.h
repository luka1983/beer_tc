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

void temperature_demo(void)
{
	int temp_int = 0, err = 0, channel_no, i;
	double temp_double = 0;
	char string_temperature[12];
	char *names[10];
	struct temperature_channel *tc = temperature_get_channel_by_name("t1");

	temperature_get_all_channel_names(&channel_no, names);
	for(i = 0; i < channel_no; i++) {
		sprints("Channel %d, name <%s>", i, names[i]);
	}

	if (tc == NULL) {
		sprints("Error - could not find channel");
		return;
	}

	err = temperature_read(tc, &temp_int);
	if (err != 0) {
		sprints("Error (%d) while temperature_read", err);
		return;
	}
	temp_double = (double)temp_int/(double)tc->result_multiplier;

	dtostrf(temp_double, 2, 4, string_temperature);
	sprints("temperature_demo: temp (int * %d) = %d, temp (double) = <%s> C",
			tc->result_multiplier, temp_int, string_temperature);
}

#endif
