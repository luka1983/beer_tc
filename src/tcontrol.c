#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial.h"
#include "tcontrol.h"
#include "commands.h"

static struct TempController controller = { .temp = 0, .temp_set = 2 };

int32_t get_ts() {
	return controller.temp_set;
}

uint8_t set_ts(int32_t ts) {
	controller.temp_set = ts;
	return 0;
}

void init_control_loop(uint32_t tc) {
	// command interface initialization
	set_command_handler(GetTs, &get_ts);
	set_command_handler(SetTs, &set_ts);

	// command loop interropt initialization
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

int32_t read_temp() {
	return controller.temp;
}

int32_t read_temp_set() {
	return controller.temp_set;
}

ISR(TIMER1_COMPA_vect) {
	// temperature control code here
	PORTB ^= 0x20;
}