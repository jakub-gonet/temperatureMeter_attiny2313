#include "../includes/settings.h"
#include "../includes/main.h"
#include "../includes/oneWire.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void initPins(void);
void setNumberInNixie(uint8_t tubeNumber, uint8_t number);
void presetNixieNumberFromLookup(uint8_t number);
void enableNixie(uint8_t tubeNumber);
uint8_t convertTemp(uint16_t temp);
uint8_t binaryToBCD(uint8_t number);

/*
 * Lookup table is used because pins ABCD of 74141 isn't connected to one port, but two instead (B, D).
 * Binary values are inversed because A is MSB and D is LSB.
 *
 * pattern: [DCBA]              0-0000 1-1000 2-0100 3-1100 4-0010 5-1010 6-0110 7-1110 8-0001 9-1001
 */
uint8_t numbersLookupTable[] = {0b000, 0b100, 0b010, 0b110, 0b001, 0b101, 0b011, 0b111, 0b000, 0b100};

uint8_t sign = 0;
uint8_t temp = 0;

int main(){
  init();
  int cycle = 0;

  while(1){
    _delay_ms(20);
    cli();
    if(!oneWire_ResetAndPresenceCheck()){
      //DS18B20 not present, show -99
      sign = 1;
      temp = 0x99;
    }
    else{
      uint8_t LSB = 0, MSB = 0;

      switch(cycle){
      case 0:
        oneWire_sendByte(0xCC); //SKIP ROM
        oneWire_sendByte(0x44); //CONVERT T
        cycle++;
        break;
      case 1:
        sei();
        _delay_ms(750);
        cli();
        cycle++;
        break;
      case 3:
        oneWire_ResetAndPresenceCheck();
        cycle++;
        break;
      case 4:
        oneWire_sendByte(0xCC); //SKIP ROM
        oneWire_sendByte(0xBE); //READ SCRATCHPAD
        cycle++;
        break;
      case 5:
        LSB = oneWire_readByte();
        MSB = oneWire_readByte();
        cycle++;
        break;
      case 6:
        oneWire_ResetAndPresenceCheck();
        cycle++;
        break;
      case 7:
        sign = (MSB>>3) & 0x1;
        temp = binaryToBCD(convertTemp((MSB<<8) | LSB));
        cycle = 0;
        break;
      }
    }
    sei();
  }
  return 0;
}

void init(void){
  initPins();
}

void initPins(void){
  //every pin as output
  DDRB = 0xFF;
  DDRD = 0xFF;
}

uint8_t convertTemp(uint16_t temp){
  if((temp>>11) & 0x1)
    temp = ~temp + 1;
  return (temp>>4) & 0x7F;
}

uint8_t binaryToBCD(uint8_t number){
  unsigned char MSD = 0;

  while (number >= 10)
    {
      number -= 10;	// hex becomes 1s (LSD)
      MSD += 0x10;  // add 1 to 10s (MSD)
    }
  return MSD + number; // pack BCD into char
}

void presetNixieNumberFromLookup(uint8_t number){
  PORTB = numbersLookupTable[number];
  if(number == 8 || number == 9)
    PORTD |= (1<<D);
  else
    PORTD &= ~(1<<D);
}

void enableNixie(uint8_t tubeNumber){
  //clear tube cathodes
  PORTD &= ~(1<<V1)
    & ~(1<<V2)
    & ~(1<<V3);

  //set choosen tube cathode
  switch(tubeNumber){
  case 1:
    PORTD |= (1<<V1);
    break;
  case 2:
    PORTD |= (1<<V2);
    break;
  case 3:
    PORTD |= (1<<V3);
    break;
  }
}

void setNumberInNixie(uint8_t tubeNumber, uint8_t number){
  presetNixieNumberFromLookup(number);
  enableNixie(tubeNumber);
}
