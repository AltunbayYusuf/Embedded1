#define ___DELAY_BACKWARD_COMPATTIBLE__
#include <util/delay.h>

void lightUpLed (int lednumber);
void lightDownLed (int lednumber);
void lightDownAllLeds();
void lightUpAllLeds();
void enableLed(int lednumber);
void enableAllLeds();
void enableLeds(uint8_t leds);
void dimLed(int lednumber,int percentage,int duration);