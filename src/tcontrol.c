#include "tcontrol.h"

static temp_controller controller = { .temp = 0, .temp_set = 0 };

void init_cl (uint32_t tc) {
	DDRB |= 0x20;

	// Cmp register value for tc in ms => OCR1A = tc * (f_cpu / 1000) / prescaler.
	// For atmega328p max prescaler value is 1024 meaning max achievable tc in ms
	// is 1024 * 2^16 / F_CPU i.e. 4194ms with F_CPU = 16MHz
	OCR1A = (uint16_t)(tc * (F_CPU / 1000) / 1024);

	TCCR1B |= (1 << WGM12);					// CTC mode on OCR1A match
	TIMSK1 |= (1 << OCIE1A);				// Trigger int on compare match
	TCCR1B |= (1 << CS12);					// Prescale with 1/1024
	return;
}

void start_cl () {
	TCCR1B |= (1 << CS10);
	return;
}

void stop_cl () {
	TCCR1B &= ~(1 << CS10);
	return;
}

uint16_t read_temp () {
	return controller.temp;
}

uint16_t read_temp_set () {
	return controller.temp_set;
}

ISR (TIMER1_COMPA_vect) {
	PORTB ^= 0x20;
}