struct TempController {
	int32_t temp;
	int32_t temp_set;
};

// init control loop, requires tc in ms
void init_control_loop(uint32_t tc);
void start_control_loop();
void stop_control_loop();
int32_t read_temp();
int32_t read_temp_set();