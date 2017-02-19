#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial.h"
#include "tcontrol.h"
#include "commands.h"
#include "temperature.h"

static struct TempController controller = { .t1 = 0, .ts = 2 };

int32_t get_ts() {
	return controller.ts;
}

uint8_t set_ts(int32_t ts) {
	controller.ts = ts;
	return 0;
}

int32_t get_t1() {
	return controller.ts;
}

void init_control_loop(uint32_t tc) {
	// command interface initialization
	set_command_handler(GetTs, &get_ts);
	set_command_handler(SetTs, &set_ts);
	set_command_handler(GetT1, &set_ts);

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

ISR(TIMER1_COMPA_vect) {
	// temperature control code here
	PORTB ^= 0x20;
}