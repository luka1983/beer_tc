#include "tcontrol.h"

static temp_controller controller = { .temp = 0, .temp_set = 0 };

void init_cl (uint16_t tc) {
	DDRB |= 0x20;

	OCR1A = 0x1E84;							// Timer match register value
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