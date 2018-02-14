#ifndef ONEWIRE_C_H
#define ONEWIRE_C_H

#include <avr/io.h>

#define P_1WIRE PD3
#define PORT_1WIRE PORTD
#define DDR_1WIRE DDRD
#define PIN_1WIRE PIND

uint8_t oneWire_ResetAndPresenceCheck(void);
void oneWire_sendByte(uint8_t byte);
uint8_t oneWire_readByte();

#endif
