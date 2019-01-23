#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include "tcontrol.h"
#include "serial.h"
#include "commands.h"
#include "lcd.h"

/* Comment out to leave 18b20 demo code out */
//#define DS18B20_DEMO

/* Define for lcd_demo */
//#define LCD_DEMO

#if defined(DS18B20_DEMO) || defined(LCD_DEMO)
#include "demo.h"
#endif

#define BOARD_ID "tca"

void get_id(char* id) {
	strcpy(id, BOARD_ID);
	return;
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
#ifdef LCD_DEMO
		lcd_demo();
#endif
	}

	return 0;
}
