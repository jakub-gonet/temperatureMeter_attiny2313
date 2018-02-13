#include "../includes/settings.h"
#include "../includes/main.h"
#include <avr/io.h>

#include <avr/interrupt.h>
#include <util/delay.h>

void initPins(void);
void init1Wire(void);

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
  /*
  * 74141 pins:
  * A - PB2
  * B - PB1
  * C - PB0
  * D - PD6
  *
  * tubes cathodes:
  * V3 - PD2
  * V2 - PD1
  * V1 - PD0
  *
  *DS18B20:
  *DQ - PD3
  */
  PORTB |= (1<<PB2)
    | (1<<PB1)
    | (1<<PB0);
  PORTD |= (1<<PD6);
}

void init1Wire(void){

}
