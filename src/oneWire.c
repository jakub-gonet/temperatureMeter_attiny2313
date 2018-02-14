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
  Sends one bit to a slave

  Pulling a bus down initializes write time slot, which lasts 60us.
  DS18B20 will read the data from 15us to end of the time slot after releasing the bus.

  To send a logical '1' sender must release the bus within 15us,
  to send a logical '0' sender must hold the bus until the time slot has ended.

  Space between two write slots should be >1us.
*/
void oneWire_sendBit(uint8_t bit){
  DDR_1WIRE |= (1<<P_1WIRE);

  _delay_us(5);
  if(bit)
    DDR_1WIRE &= ~(1<<P_1WIRE);

  _delay_us(80);
  DDR_1WIRE &= ~(1<<P_1WIRE);
}

/*
  Sends one byte to slave
*/
void oneWire_sendByte(uint8_t byte){
  for(uint8_t i=0; i < 8; i++){
    oneWire_sendBit((byte >> i) & 0x1);
  }

}
/*
  Reads one bit from a slave

  Pulling a bus down initializes read time slot, which lasts 60us.

  DS18B20 will transmit the data after releasing the bus.
  Data is valid up to 15us after releasing the bus.

  Space between two read slots should be >1us.
*/
uint8_t oneWire_readBit(){
  DDR_1WIRE |= (1<<P_1WIRE);
  _delay_us(2);
  DDR_1WIRE &= ~(1<<P_1WIRE);
  _delay_us(10);
  uint8_t bitState = PIN_1WIRE & P_1WIRE;
  _delay_us(50);
  return bitState;
}

/*
  Reads one byte from slave
*/
uint8_t oneWire_readByte(){
  uint8_t buffer = 0;
  for(uint8_t i=0; i < 8; i++){
    buffer |= oneWire_readBit() << i;
  }
  return buffer;
}
