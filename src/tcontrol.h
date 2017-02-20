#include "temperature.h"
#include "gpio.h"

typedef enum { Stopped, Started, Err } TCState;

struct TempController {
	int32_t ts;							// control loop set temperature
	int32_t hl;							// hysteresis lower threshold
	int32_t hu;							// hysteresis upper threshold
	struct temperature_channel* t1;		// main temperature sensor
	struct temperature_channel* t2;		// secondary temperature sensor
	TCState state;
	struct Pin out;
};

// init control loop, requires tc in ms
void init_control_loop(uint32_t tc);
void start_control_loop();
void stop_control_loop();

// controller set temperature
int32_t get_ts();
uint8_t set_ts();

// controller temperature readings
int32_t get_t1();
int32_t get_t2();