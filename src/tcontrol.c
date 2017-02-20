#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial.h"
#include "tcontrol.h"
#include "commands.h"

static struct TempController controller = { .t1 = NULL, .t2 = NULL, .ts = 21 * DEC_DIV };

int32_t get_ts() {
	return controller.ts;
}

uint8_t set_ts(int32_t ts) {
	controller.ts = ts;
	return 0;
}

int32_t get_t1() {
	return controller.t1->temp_c;
}

int32_t get_t2() {
	return controller.t2->temp_c;
}

void init_control_loop(uint32_t tc) {
	// read temperature channel configuration
	controller.t1 = temperature_get_channel_by_name("t1");
	controller.t2 = temperature_get_channel_by_name("t2");

	// command interface initialization
	set_command_handler(GetTs, &get_ts);
	set_command_handler(SetTs, &set_ts);
	set_command_handler(GetT1, &get_t1);
	set_command_handler(GetT2, &get_t2);

	// command loop interrupt initialization
	DDRB |= 0x20;
	TCCR1B |= (1 << WGM12);					// CTC mode on OCR1A match
	TIMSK1 |= (1 << OCIE1A);				// Trigger int on compare match

	// Cmp register value for tc in ms => OCR1A = tc * (f_cpu / 1000) / prescaler.
	// For atmega328p max prescaler value is 1024 meaning max achievable tc in ms
	// is 1024 * 2^16 / F_CPU i.e. 4194ms with F_CPU = 16MHz
	OCR1A = (uint16_t)(tc * (F_CPU / 1000) / 1024);

	return;
}

void start_control_loop() {
	TCCR1B |= (1 << CS10) | (1 << CS12);	// Set prescaler (starts timer)
	return;
}

void stop_control_loop() {
	TCCR1B &= ~((1 << CS10) | (1 << CS12));
	return;
}

ISR(TIMER1_COMPA_vect) {
	// temperature control code here
	PORTB ^= 0x20;
	temperature_read(controller.t1, 1);
	temperature_read(controller.t2, 1);
	controller.t1->temp_c = DEC_DIV * controller.t1->temp_raw / controller.t1->result_multiplier;
	controller.t2->temp_c = DEC_DIV * controller.t2->temp_raw / controller.t2->result_multiplier;
}
