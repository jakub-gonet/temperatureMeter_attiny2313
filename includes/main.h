#ifndef MAIN_C_H
#define MAIN_C_H
#include <avr/io.h>

//74141 pins
#define A PB2
#define B PB1
#define C PB0
#define D PD6

//tubes cathodes
#define V3 PD2
#define V2 PD1
#define V1 PD0

//DS18B20
#define DQ PD3

void init(void);

#endif
