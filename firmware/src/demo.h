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
#include "lcd.h"

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
	int err = 0, channel_no, i;
	double temp_double = 0;
	char string_temperature[12];
	char **names;
	struct temperature_channel **tc;

	channel_no = temperature_get_nr_channels();
	if (channel_no == 0) {
		sprints("Error - no channels configured\r\n");
		return;
	}
	names = (char **) malloc (sizeof(char*) * channel_no);

	temperature_get_all_channel_names(&channel_no, names);

	tc = (struct temperature_channel **) malloc (sizeof(struct temperature_channel*) * channel_no);

	for(i = 0; i < channel_no; i++) {
		tc[i] = temperature_get_channel_by_name(names[i]);
		if (tc[i] == NULL) {
			sprints("Could not find channel <%s>\r\n", names[i]);
		}
	}

	for(i = 0; i < channel_no; i++) {
		err = temperature_read(tc[i], 0);
		if (err != 0) {
			sprints("channel %s: error (%d) while temperature_read\r\n", tc[i]->name, err);
			continue;
		}
		temp_double = (double)tc[i]->temp_raw/(double)tc[i]->result_multiplier;

		dtostrf(temp_double, 2, 4, string_temperature);
		sprints("channel %s: temperature: %s C\r\n", tc[i]->name, string_temperature);
	}
	free(tc);
	free(names);
}

static const PROGMEM unsigned char copyRightChar[] =
{
	0x07, 0x08, 0x13, 0x14, 0x14, 0x13, 0x08, 0x07,
	0x00, 0x10, 0x08, 0x08, 0x08, 0x08, 0x10, 0x00
};

void lcd_demo(void)
{
	char buffer[7];
	int  num=134;
	unsigned char i;

	DDRB = DDRB | (1 << 5);
	PORTB = PORTB | (1 << 5);

	/* initialize display, cursor off */
	lcd_init(LCD_DISP_ON);

	for (;;) {
		/*
		 * Test 1:	write text to display
		 */
		/* clear display and home cursor */
		lcd_clrscr();

		/* put string to display (line 1) with linefeed */
		lcd_puts("LCD Test Line 1\n");

		/* cursor is now on second line, write second line */
		lcd_puts("Line 2");

		/* move cursor to position 8 on line 2 */
		lcd_gotoxy(7,1);

		/* write single char to display */
		lcd_putc(':');

		/* wait */
		_delay_ms(5000);
		PORTB = PORTB ^ (1 << 5);

		/*
		 * Test 2: use lcd_command() to turn on cursor
		 */

		/* turn on cursor */
		lcd_command(LCD_DISP_ON_CURSOR);

		/* put string */
		lcd_puts( "CurOn");

		/* wait */
		_delay_ms(5000);
		PORTB = PORTB ^ (1 << 5);

		/*
		 * Test 3: display shift
		 */
		lcd_clrscr();	  /* clear display home cursor */

		/* put string from program memory to display */
		lcd_puts_P( "Line 1 longer than 14 characters\n" );
		lcd_puts_P( "Line 2 longer than 14 characters" );

		/* move BOTH lines one position to the left */
		lcd_command(LCD_MOVE_DISP_LEFT);

		/* wait */
		_delay_ms(5000);

		/* turn off cursor */
		lcd_command(LCD_DISP_ON);


		/*
		 *	 Test: Display integer values
		 */
		lcd_clrscr();	/* clear display home cursor */

		/* convert interger into string */
		itoa( num , buffer, 10);

		/* put converted string to display */
		lcd_puts(buffer);

		/* wait */
		_delay_ms(5000);
		PORTB = PORTB ^ (1 << 5);

		/*
		 *	Test: Display userdefined characters
		 */
		lcd_clrscr();   /* clear display home cursor */

		lcd_puts("Copyright: ");

		/*
		 * load two userdefined characters from program memory
		 * into LCD controller CG RAM location 0 and 1
		 */
		lcd_command(_BV(LCD_CGRAM));  /* set CG RAM start address 0 */
		for(i=0; i<16; i++)
		{
			lcd_data(pgm_read_byte_near(&copyRightChar[i]));
		}

		/* move cursor to position 0 on line 2 */
		/* Note: this switched back to DD RAM adresses */
		lcd_gotoxy(0,1);

		/* display user defined (c), built using two user defined chars */
		lcd_putc(0);
		lcd_putc(1);

		/* wait */
		_delay_ms(5000);
		PORTB = PORTB ^ (1 << 5);
	}
}

#endif
