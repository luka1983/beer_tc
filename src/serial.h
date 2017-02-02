#include <avr/io.h>
#include <avr/interrupt.h>

void init_serial(uint16_t brate, uint8_t dbits, uint8_t parity, uint8_t sbits);
void sputc(const char c);
void sprints(const char* fmt, ...);