#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <led.h>
#include <usart.h>
#include <button.h>
#include <display.h>

void showMorseCode(int nummer)
{
  if (nummer == 1)
  {
    lightUpAllLeds();
    _delay_ms(500);
    lightDownAllLeds();
  }
  else if (nummer == 2)
  {
    lightUpAllLeds();
    _delay_ms(1000);
    lightDownAllLeds();
  }
}

void showCode(char *showMorse)
{
  for (size_t i = 0; showMorse[i] != '\0'; i++)
  {
    _delay_ms(1000);
    printf(showMorse[i]);
    if (showMorse[i] == '.')
    {
      showMorseCode(1);
    }
    else if (showMorse[i] == '-')
    {
      showMorseCode(2);
    }
  }
}

void dansje()
{
  // Stap 1: buitenste leds aan
  lightUpLed(0);
  lightUpLed(3);
  _delay_ms(100);

  // Stap 2: buitenste leds uit, binnenste leds aan
  lightDownLed(0);
  lightDownLed(3);
  lightUpLed(1);
  lightUpLed(2);
  _delay_ms(100);

  // Stap 3: alle leds aan
  lightUpLed(0);
  lightUpLed(3);
  _delay_ms(100);

  // Stap 4: alle leds uit
  lightDownLed(0);
  lightDownLed(1);
  lightDownLed(2);
  lightDownLed(3);
  _delay_ms(100);

  // Stap 5: loop rondje vooruit
  lightUpLed(0);
  _delay_ms(80);
  lightDownLed(0);
  lightUpLed(1);
  _delay_ms(80);
  lightDownLed(1);
  lightUpLed(2);
  _delay_ms(80);
  lightDownLed(2);
  lightUpLed(3);
  _delay_ms(80);
  lightDownLed(3);

  // Stap 6: loop rondje achteruit
  lightUpLed(3);
  _delay_ms(80);
  lightDownLed(3);
  lightUpLed(2);
  _delay_ms(80);
  lightDownLed(2);
  lightUpLed(1);
  _delay_ms(80);
  lightDownLed(1);
  lightUpLed(0);
  _delay_ms(80);
  lightDownLed(0);

  // Stap 7: knipperen alle leds 2 keer
  for (int i = 0; i < 2; i++)
  {
    lightUpLed(0);
    lightUpLed(1);
    lightUpLed(2);
    lightUpLed(3);
    _delay_ms(100);
    lightDownLed(0);
    lightDownLed(1);
    lightDownLed(2);
    lightDownLed(3);
    _delay_ms(100);
  }

  // Stap 8: binnenste leds aan, dan buitenste aan
  lightUpLed(1);
  lightUpLed(2);
  _delay_ms(100);
  lightUpLed(0);
  lightUpLed(3);
  _delay_ms(100);

  // Stap 9: uit van binnen naar buiten
  lightDownLed(1);
  lightDownLed(2);
  _delay_ms(100);
  lightDownLed(0);
  lightDownLed(3);
}

int main()
{

  char morseCode[26][5] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--",
                           "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."};

  int randomizer;
  char letter;

  initUSART();
  enableAllLeds();
  for (size_t i = 0; i < 4; i++)
  {
    _delay_ms(1000);
    lightDownLed(i);
  }

  for (size_t i = 0; i < 10; i++)
  {
    randomizer = rand() % 26;
    letter = 'A' + randomizer;
    printf("Morse code : %s \n", morseCode[randomizer]);
    showCode(morseCode[randomizer]);
    printf("Je hebt 3 seconden voor je de oplossing te zien krijgt.\n");
    printf("3\n");
    _delay_ms(1000);
    printf("2\n");
    _delay_ms(1000);
    printf("1\n");
    _delay_ms(1000);
    printf("0\n");
    printf("Het was de letter %c\n", letter);
  }

  dansje();

  return 0;
}
