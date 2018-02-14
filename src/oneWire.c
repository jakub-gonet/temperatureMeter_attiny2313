#include "../includes/settings.h"
#include "../includes/oneWire.h"
#include <avr/io.h>
#include <util/delay.h>

uint8_t oneWire_readBit();
void oneWire_writeBit(uint8_t bit);

/*
   Resets 1 wire line, then checks if any slave DS18B20 is present

   Documentation, page 15:
   https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf
*/
uint8_t oneWire_ResetAndPresenceCheck(void){
  PORT_1WIRE &= ~(1<<P_1WIRE);
  DDR_1WIRE |= (1<<P_1WIRE);
  _delay_us(500);
  DDR_1WIRE &= ~(1<<P_1WIRE);
  _delay_us(50);

  uint8_t presence = PIN_1WIRE & P_1WIRE;
  _delay_us(450);
  return !presence;
}

/*
  Sends one bit to slave
*/
void oneWire_writeBit(uint8_t bit){

}

/*
  Sends one byte to slave
*/
void oneWire_sendByte(uint8_t byte){

}
/*
  Reads one bit from slave
*/
uint8_t oneWire_readBit(){

}

/*
  Reads one byte from slave
*/
uint8_t oneWire_readByte(){

}
