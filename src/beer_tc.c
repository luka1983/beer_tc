#include <avr/io.h>
#include <avr/interrupt.h>

#include "tcontrol.h"

int main (void) {
	sei();			// Enable interrupts
	init_cl(1000);
	start_cl();

	for(;;) {}

	return 0;
}