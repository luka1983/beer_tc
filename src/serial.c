#include "serial.h"

void init_serial(uint16_t brate, uint8_t dbits, uint8_t parity, uint8_t sbits) {
	uint16_t ubrr0 = (uint16_t) (F_CPU / (brate * 16UL)) - 1;
	UBRR0H = (uint8_t)(ubrr0 >> 8);				// configure baud rate
	UBRR0L = (uint8_t)(ubrr0);
	UCSR0C |= (1 << UCSZ01) | (1 <<UCSZ00);		// configure data bits, parity and stop bits
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);		// enable RX/TX
	UCSR0B |= (1 << RXCIE0);					// enable interrupt on RX
}

void crnl() {
	sputc('\r');
	sputc('\n');
}

void sputc(const char c) {
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
}

void sprints(const char* str) {
	while (*str != '\0')
		sputc(*(str++));
	crnl();
}

ISR(USART_RX_vect) {
	uint8_t rb = UDR0;
	sputc(rb);
	if (rb == '\r')
		sputc('\n');
}