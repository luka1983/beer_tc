struct TempController {
	int32_t ts;
	int32_t t1;
	int32_t t2;
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