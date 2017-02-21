#include <stdint.h>

typedef enum { On, Off } PinState;
typedef enum { In, Out } PinDir;
typedef enum { Low, High } PinActive;

struct Pin {
	volatile uint8_t* port;
	volatile uint8_t* dirreg;
	uint8_t num;
	PinDir dir;
	PinActive active;
    PinState init;
};

void pin_init(struct Pin pin);
void pin_on(struct Pin pin);
void pin_off(struct Pin pin);
void pin_toggle(struct Pin pin);
PinState pin_state(struct Pin pin);