/* Comment out to leave 18b20 demo code out */
#define DS18B20_DEMO

#include <avr/io.h>
#include <avr/interrupt.h>
#include "tcontrol.h"
#include "serial.h"

#ifdef DS18B20_DEMO
#include <util/delay.h>
#include <stdlib.h>
#include "temperature.h"
#include "temperature_channel_config.h"

void temperature_demo(void);
#endif

int main(void) {
	sei();			// Enable interrupts
	init_control_loop(1000);
	init_serial(19200, 8, 0, 1);
	start_control_loop();

	for(;;) {
#ifdef DS18B20_DEMO
		temperature_demo();
#endif
	}

	return 0;
}

#ifdef DS18B20_DEMO
void temperature_demo(void)
{
	int temp_int = 0, err = 0;
	double temp_double = 0;
	char string_temperature[32];

	err = temperature_read(&channel_1, &temp_int);
	if (err != 0) {
		sprints("Error (%d) while temperature_read", err);
		return;
	}
	temp_double = (double)temp_int/(double)channel_1.result_multiplier;
	dtostrf(temp_double, 2, 4, string_temperature);
	sprints("temperature_demo: temp (int * %d) = %d, temp (double) = %s",
			channel_1.result_multiplier, temp_int, string_temperature);
}
#endif
