#include "../includes/settings.h"
#include "../includes/main.h"
#include "../includes/oneWire.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

inline void init(void);
void setNumberInNixie(uint8_t tubeNumber, uint8_t number);
inline void presetNixieNumberFromLookup(uint8_t number);
inline void enableNixie(uint8_t tubeNumber);
inline uint8_t convertTemp(uint16_t temp);
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
uint8_t LSB = 0, MSB = 0;
uint8_t numOfOverflows = 0;

int main(){
  init();
  uint8_t currentTube = 0;

  oneWire_ResetAndPresenceCheck();
  oneWire_sendByte(0xCC); //SKIP ROM
  oneWire_sendByte(0x44); //CONVERT T

  while(1){
    switch(currentTube){
    case 0:
      setNumberInNixie(0, sign);
      currentTube++;
      break;
    case 1:
      setNumberInNixie(1, temp>>4);
      currentTube++;
      break;
    case 2:
      setNumberInNixie(2, temp & 0x0F);
      currentTube++;
      break;
    default:
      currentTube = 0;
    }
  }
}

void init(void){
  DDRB = 0xFF;
  DDRD = 0xFF;

  //calculated to be 1s:
  //(F_CPU/OCR0A/prescaler)
  //1000000/1024/255 = ~4Hz for each tube
  OCR0A = 255;
  TCCR0A = (1<<WGM01);// Clear Timer on Compare Match (CTC) mode
  TIMSK = (1<<OCIE0A);// TC0 compare match A interrupt enable
  TCCR0B = (1<<CS02) | (1<<CS00);//prescaler 1024
  sei();
}

uint8_t convertTemp(uint16_t temp){
  if((temp>>11) & 0x1)
    temp = ~temp + 1;
  return (temp>>4) & 0x7F;
}

uint8_t binaryToBCD(uint8_t number){
  uint8_t MSD = 0;

  while (number >= 10){
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
  PORTD = ~(1<<V1)
    & ~(1<<V2)
    & ~(1<<V3);

  //set choosen tube cathode
  switch(tubeNumber){
  case 0:
    PORTD |= (1<<V1);
    break;
  case 1:
    PORTD |= (1<<V2);
    break;
  case 2:
    PORTD |= (1<<V3);
  }
}

void setNumberInNixie(uint8_t tubeNumber, uint8_t number){
  presetNixieNumberFromLookup(number);
  enableNixie(tubeNumber);
}

ISR(TIMER0_COMPA_vect){
  numOfOverflows++;
  if(numOfOverflows >= 4){
    if(oneWire_ResetAndPresenceCheck()){
      oneWire_sendByte(0xCC); //SKIP ROM
      oneWire_sendByte(0xBE); //READ SCRATCHPAD
      oneWire_ResetAndPresenceCheck();
      LSB = oneWire_readByte();
      MSB = oneWire_readByte();
      sign = (MSB>>3) & 0x1;
      temp = binaryToBCD(convertTemp((MSB<<8) | LSB));

      oneWire_ResetAndPresenceCheck();
      oneWire_sendByte(0xCC); //SKIP ROM
      oneWire_sendByte(0x44); //CONVERT T
    }
    else{
      //DS18B20 not present, show ~00
      sign = 2;
      temp = 0x00;
    }

    numOfOverflows = 0;
  }
}
