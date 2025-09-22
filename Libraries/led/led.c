#include <avr/io.h>
#include <util/delay.h>


void lightUpLed(int lednumber){
    PORTB &= ~(1<<(PB2 + lednumber));
}
  
void lightDownLed(int lednumber){
    PORTB|= (1<<(PB2 + lednumber));
}
  
void enableLed(int lednumber){
    if(lednumber<0|| lednumber>3){
      return;
    }
    DDRB|=(1<<(PB2+lednumber));
}
  
void enableAllLeds(){
    for (uint8_t i = 0; i < 4; i++){
        enableLed(i);
    }
}

void enableLeds(uint8_t leds){ //bv 00001111 alle leds 00001001
    DDRB |= (leds<<PB2);
}

void dimLed(int lednumber, int percentage, int duration){
    for(int i = 0; i < duration; i++){
        lightUpLed(lednumber);
        _delay_us(percentage/10);
        lightDownLed(lednumber);
        _delay_us((10-(percentage/10)));
    }
}

void lightDownAllLeds(){
    for (uint8_t i = 0; i < 4; i++){
        lightDownLed(i);
    }
}

void lightUpAllLeds(){
    for (uint8_t i = 0; i < 4; i++){
        lightUpLed(i);
    }
}