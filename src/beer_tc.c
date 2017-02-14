/* Comment out to leave 18b20 demo code out */
#define DS18B20_DEMO

#include <avr/io.h>
#include <avr/interrupt.h>
#include "tcontrol.h"
#include "serial.h"

#ifdef DS18B20_DEMO
#include <util/delay.h>
#include <stdlib.h>
#include "ds18b20.h"

void ds18b20_demo(volatile uint8_t *ds18b20_port,
		  volatile uint8_t *ds18b20_direction,
		  volatile uint8_t *ds18b20_portin,
		  uint8_t pin_mask);
#endif

int main(void) {
	sei();			// Enable interrupts
	init_control_loop(1000);
	init_serial(19200, 8, 0, 1);
	start_control_loop();

	for(;;) {
#ifdef DS18B20_DEMO
		ds18b20_demo(&PORTD, &DDRD, &PIND, 1 << 6);
#endif
	}

	return 0;
}

#ifdef DS18B20_DEMO
/*
 * Demo function for reading temperature from DS18B20.
 * port, direction, portin, pinmask specify port and
 * pin to which sensor is connected
 * */
void ds18b20_demo(volatile uint8_t *ds18b20_port,
		  volatile uint8_t *ds18b20_direction,
		  volatile uint8_t *ds18b20_portin,
		  uint8_t pin_mask)
{
	int temp_int = 0, err = 0;
	static unsigned int cntr = 0;
	double temp_double = 0;
	char string_temperature[32];

	cntr++;
	err = ds18b20convert(ds18b20_port, ds18b20_direction,
			     ds18b20_portin, pin_mask, NULL);
	if (err != 0) {
		sprints("Error (%d) while issuing CONVERT COMMAND", err);
		return;
	}

	/* see ds18b20 ds, temp conversion in 12 bit resolution
	 * can last up to 750 ms */
	_delay_ms(1000);

	err = ds18b20read(ds18b20_port, ds18b20_direction,
			  ds18b20_portin, pin_mask, NULL, &temp_int);
	if (err != 0) {
		sprints("Error (%d) while issuing READ COMMAND", err);
		return;
	}

	temp_double = (double)temp_int/(double)DS18B20_MUL;
	dtostrf(temp_double, 2, 4, string_temperature);
	sprints("[%d] temp (int * %d) = %d, temp (double) = %s",
			cntr, DS18B20_MUL, temp_int, string_temperature);
}
#endif
