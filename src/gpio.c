#include "gpio.h"

void pin_init(struct Pin pin) {
	if (pin.dir == In)
		*(pin.dirreg) &= ~(1 << pin.num);
	else if (pin.dir == Out)
		*(pin.dirreg) |= 1 << pin.num;
}

void pin_on(struct Pin pin) {
	if (pin.active == Low)
		*(pin.port) &= ~(1 << pin.num);
	else if (pin.active == High)
		*(pin.port) |= 1 << pin.num;
}

void pin_off(struct Pin pin) {
	if (pin.active == Low)
		*(pin.port) |= 1 << pin.num;
	else if (pin.active == High)
		*(pin.port) &= ~(1 << pin.num);
}

void pin_toggle(struct Pin pin) {
	*(pin.port) ^= 1 << pin.num;
}

PinState pin_state(struct Pin pin) {
	if (pin.active == Low)
        return (*(pin.port) & (1 << pin.num)) ? Off : On;
	else
        return (*(pin.port) & (1 << pin.num)) ? On : Off;
}