#include <avr/io.h>
#include <avr/interrupt.h>

#include "tcontrol.h"
#include "serial.h"

int main(void) {
	sei();			// Enable interrupts
	init_control_loop(1000);
	init_serial(19200, 8, 0, 1);
	start_control_loop();

	for(;;) {}

	return 0;
}