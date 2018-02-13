#include "../includes/settings.h"
#include "../includes/main.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void initPins(void);
void init1Wire(void);
void setNumberInNixie(uint8_t tubeNumber, uint8_t number);
void presetNixieNumberFromLookup(uint8_t number);
void enableNixie(uint8_t tubeNumber);

/*
 * Lookup table is used because pins ABCD of 74141 isn't connected to one port, but two instead (B, D).
 * Binary values are inversed, because A is MSB and D is LSB.
 *
 * pattern: [DCBA]              0-0000 1-1000 2-0100 3-1100 4-0010 5-1010 6-0110 7-1110 8-0001 9-1001
 */
uint8_t numbersLookupTable[] = {0b000, 0b100, 0b010, 0b110, 0b001, 0b101, 0b011, 0b111, 0b000, 0b100};

int main(){
  init();
  while(1){

  }
  return 0;
}

void init(void){
  initPins();
  init1Wire();
}

void initPins(void){
  //every pin as output
  DDRB = 0xff;
  DDRD = 0xff;
}

void init1Wire(void){

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
