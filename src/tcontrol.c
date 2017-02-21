#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial.h"
#include "tcontrol.h"
#include "commands.h"

static struct TempController tc = {
	.ct = 1000,
	.ts = 21 * DEC_DIV,
	.hl = 5 * DEC_DIV / 10,
	.hu = 5 * DEC_DIV / 10,
	.t1 = NULL,
	.t2 = NULL,
	.state = Stopped,
	.out = {
		.port = &PORTD,
		.dirreg = &DDRD,
		.num = 2,
		.dir = Out,
		.active = Low,
		.init = Off
	},
	.cdc = 0,
	.cdt = 10000
};

static struct Pin led = {
	.port = &PORTB,
	.dirreg = &DDRB,
	.num = 5,
	.dir = Out,
	.active = High,
	.init = On
};

int32_t get_ts() {
	return tc.ts;
}

uint8_t set_ts(int32_t ts) {
	tc.ts = ts;
	return 0;
}

int32_t get_t1() {
	return tc.t1->temp_c;
}

int32_t get_t2() {
	return tc.t2->temp_c;
}

int32_t get_co() {
	return pin_state(tc.out) == On ? 1 : 0;
}

void init_control_loop(uint32_t ct) {
	tc.ct = ct;

	// controller digital output init
	pin_init(tc.out);

	// led pin init_cl
	pin_init(led);

	// read temperature channel configuration
	tc.t1 = temperature_get_channel_by_name("t1");
	tc.t2 = temperature_get_channel_by_name("t2");

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
	OCR1A = (uint16_t)(ct * (F_CPU / 1000) / 1024);

	return;
}

void start_control_loop() {
	tc.state = Started;
	tc.cdc = tc.cdt * tc.ct;
	TCCR1B |= (1 << CS10) | (1 << CS12);	// Set prescaler (starts timer)
	return;
}

void stop_control_loop() {
	tc.state = Stopped;
	TCCR1B &= ~((1 << CS10) | (1 << CS12));
	return;
}

void set_control_out(PinState out) {
	tc.cdc = 0;
	// inverted output
	if (out == On)
		pin_on(tc.out);
	else if (out == Off)
		pin_off(tc.out);
}

void update_control_timers() {
	tc.cdc++;
	return;
}

void update_temperature_readings() {
	temperature_read(tc.t1, 1);
	temperature_read(tc.t2, 1);
	tc.t1->temp_c = DEC_DIV * tc.t1->temp_raw / tc.t1->result_multiplier;
	tc.t2->temp_c = DEC_DIV * tc.t2->temp_raw / tc.t2->result_multiplier;
}

void update_controller() {
	update_control_timers();
	if (tc.cdc < tc.cdt / tc.ct) return;
	if ((tc.t1->temp_c > tc.ts + tc.hu) && pin_state(tc.out) == Off)
		set_control_out(On);
	else if ((tc.t1->temp_c < tc.ts - tc.hl) && pin_state(tc.out) == On)
		set_control_out(Off);
}

void update_control_loop() {
	update_temperature_readings();

	switch (tc.state) {
		case Stopped:
		case Err:
			break;
		case Started:
			tc.state = Initializing;
			break;
		case Initializing:
			tc.state = Running;
			break;
		case Running:
			update_controller();
	}
	return;
}

ISR(TIMER1_COMPA_vect) {
	// temperature control code here
	pin_toggle(led);
	update_control_loop();
}
