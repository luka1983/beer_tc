#include <avr/io.h>
#include <avr/interrupt.h>
#include "tcontrol.h"
#include "serial.h"
#include "commands.h"

/* Comment out to leave 18b20 demo code out */
//#define DS18B20_DEMO

#ifdef DS18B20_DEMO
#include "demo.h"
#endif

#define BOARD_ID 123456

uint32_t get_id() {
	return BOARD_ID;
}

int main(void) {
	sei();			// Enable interrupts
	init_control_loop(1000);
	init_serial(19200, 8, 0, 1);

	set_command_handler(GetId, &get_id);

	start_control_loop();

	for(;;) {
#ifdef DS18B20_DEMO
		temperature_demo();
#endif
	}

	return 0;
}
