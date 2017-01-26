#include <avr/io.h>
#include <avr/interrupt.h>

typedef struct temp_controller {
	uint16_t temp;
	uint16_t temp_set;
} temp_controller;

// init control loop, requires tc in ms
void init_control_loop(uint32_t tc);
void start_control_loop();
void stop_control_loop();
uint16_t read_temp();
uint16_t read_temp_set();