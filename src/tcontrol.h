#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

typedef struct temp_controller {
	uint16_t temp;
	uint16_t temp_set;
} temp_controller;

// init control loop, requires Tc in ms
void init_cl (uint16_t tc);
void start_cl ();
void stop_cl ();
uint16_t read_temp ();
uint16_t read_temp_set ();