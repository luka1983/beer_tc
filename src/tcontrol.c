#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial.h"
#include "tcontrol.h"
#include "commands.h"

static struct TempController ctrl = {
	.ts = 21 * DEC_DIV,
	.hl = 5 * DEC_DIV / 10,
	.hu = 5 * DEC_DIV / 10,
	.t1 = NULL,
	.t2 = NULL,
	.state = Stopped,
	.out = {
		.port = &PORTB,
		.dirreg = &DDRB,
		.num = 1,
		.dir = Out,
		.active = High
	}
};

static struct Pin led = {
	.port = &PORTB,
	.dirreg = &DDRB,
	.num = 5,
	.dir = Out,
	.active = High
};

int32_t get_ts() {
	return ctrl.ts;
}

uint8_t set_ts(int32_t ts) {
	ctrl.ts = ts;
	return 0;
}

int32_t get_t1() {
	return ctrl.t1->temp_c;
}

int32_t get_t2() {
	return ctrl.t2->temp_c;
}

int32_t get_co() {
	return pin_state(ctrl.out) == On ? 1 : 0;
}

void init_control_loop(uint32_t tc) {
	// controller digital output init
	pin_init(ctrl.out);

	// led pin init_cl
	pin_init(led);

	// read temperature channel configuration
	ctrl.t1 = temperature_get_channel_by_name("t1");
	ctrl.t2 = temperature_get_channel_by_name("t2");

	// command interface initialization
	set_command_handler(GetTs, &get_ts);
	set_command_handler(SetTs, &set_ts);
	set_command_handler(GetT1, &get_t1);
	set_command_handler(GetT2, &get_t2);
	set_command_handler(GetCo, &get_co);

	// command loop interrupt initialization
	TCCR1B |= (1 << WGM12);					// CTC mode on OCR1A match
	TIMSK1 |= (1 << OCIE1A);				// Trigger int on compare match

	// Cmp register value for tc in ms => OCR1A = tc * (f_cpu / 1000) / prescaler.
	// For atmega328p max prescaler value is 1024 meaning max achievable tc in ms
	// is 1024 * 2^16 / F_CPU i.e. 4194ms with F_CPU = 16MHz
	OCR1A = (uint16_t)(tc * (F_CPU / 1000) / 1024);

	return;
}

void start_control_loop() {
	ctrl.state = Started;
	TCCR1B |= (1 << CS10) | (1 << CS12);	// Set prescaler (starts timer)
	return;
}

void stop_control_loop() {
	ctrl.state = Stopped;
	TCCR1B &= ~((1 << CS10) | (1 << CS12));
	return;
}

void set_control_out(struct TempController* tc, PinState out) {
	// inverted output
	if (out == On)
		pin_on(tc->out);
	else if (out == Off)
		pin_off(tc->out);
}

void update_control_timers() {
	return;
}

void update_control_loop() {
	update_control_timers();
	temperature_read(ctrl.t1, 1);
	temperature_read(ctrl.t2, 1);

	ctrl.t1->temp_c = DEC_DIV * ctrl.t1->temp_raw / ctrl.t1->result_multiplier;
	ctrl.t2->temp_c = DEC_DIV * ctrl.t2->temp_raw / ctrl.t2->result_multiplier;

	if ((ctrl.t1->temp_c > ctrl.ts + ctrl.hu) && pin_state(ctrl.out) == Off)
		set_control_out(&ctrl, On);
	else if ((ctrl.t1->temp_c < ctrl.ts - ctrl.hl) && pin_state(ctrl.out) == On)
		set_control_out(&ctrl, Off);
}

ISR(TIMER1_COMPA_vect) {
	// temperature control code here
	pin_toggle(led);
	update_control_loop();
}
