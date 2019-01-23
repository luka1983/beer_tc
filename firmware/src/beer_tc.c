#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include <stdlib.h>
#include "tcontrol.h"
#include "serial.h"
#include "commands.h"
#include "lcd.h"
#include "version.h"

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

void get_version(char* version) {
	strcpy(version, FIRMWARE_VERSION);
	return;
}

void lcd_update_tc(void)
{
	int err = 0;
	double temp_double = 0;
	char string_temperature[12];
	struct temperature_channel *tc = temperature_get_channel_by_name("t1");
	err = temperature_read(tc, 0);
	if (err != 0) {
		strcpy(string_temperature, "ERROR");
	} else {
		temp_double = (double)tc->temp_raw/(double)tc->result_multiplier;
		dtostrf(temp_double, 2, 4, string_temperature);
	}
	lcd_gotoxy(0,0);
	lcd_puts("Tc: ");
	lcd_puts(string_temperature);
}

void lcd_update_ts(void)
{
	int err = 0;
	int ts = get_ts();
	double temp_double = 0;
	char string_temperature[12];
	temp_double = (double)ts/(double)DEC_DIV;
	dtostrf(temp_double, 2, 4, string_temperature);
	lcd_gotoxy(0,1);
	lcd_puts("Ts: ");
	lcd_puts(string_temperature);
}

int main(void) {
	sei();			// Enable interrupts
	init_control_loop(1000);
	init_serial(19200, 8, 0, 1);

	set_command_handler(GetId, &get_id);
	set_command_handler(GetVersion, &get_version);

	lcd_init(LCD_DISP_ON);
	lcd_clrscr();

	start_control_loop();

	for(;;) {
#ifdef DS18B20_DEMO
		temperature_demo();
#endif
#ifdef LCD_DEMO
		lcd_demo();
#endif
		lcd_update_tc();
		lcd_update_ts();
		_delay_ms(1000);
	}

	return 0;
}
