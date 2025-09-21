#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#include <led.h>
#include <button.h>
#include <usart.h>

#define __DELAY_BACKWARD_COMPATIBLE__
#define MAX_LENGTH 10
#define DELAY 500

volatile int button_pushed = 0; // wordt gezet via ISR
volatile int seedCounter = 0;

// ISR: wordt uitgevoerd bij een druk op een knop
ISR(PCINT1_vect)
{
    if (buttonPushed(0))
    { // alleen knop1 mag het spel starten
        button_pushed = 1;
    }
}

//spel opstarten en seeden
void waitForStart()
{
    button_pushed = 0;
    seedCounter = 0;

    while (!button_pushed)
    {
        lightUpLed(3);
        _delay_ms(50);
        lightDownLed(3);
        _delay_ms(50);

        seedCounter++;

        if (buttonPushed(0))
        {
            button_pushed = 1;
        }
    }

    srand(seedCounter);
    lightDownAllLeds();
}

//aanmaken van random patroon
void generatePuzzle(uint8_t *pattern, int length)
{
    for (int i = 0; i < length; i++)
    {
        pattern[i] = rand() % 3;
    }
}

//print patroon naar serial monitor
void printPuzzle(uint8_t *pattern, int length)
{
    printf("[ ");
    for (int i = 0; i < length; i++)
    {
        printf("%d ", pattern[i] + 1);
    }
    printf("]\n");
}

//pattern afspelen op leds
void playPuzzle(uint8_t *pattern, int length)
{
    for (int i = 0; i < length; i++)
    {
        lightUpLed(pattern[i]);
        _delay_ms(DELAY);
        lightDownLed(pattern[i]);
        _delay_ms(DELAY);
    }
}

// Feedback-led (led4 flikkert)
void flashFeedbackLed(int times)
{
    for (int i = 0; i < times; i++)
    {
        lightUpLed(3);
        _delay_ms(150);
        lightDownLed(3);
        _delay_ms(150);
    }
}

//uitlezen input gebruiker
int readInput(uint8_t *pattern, int length)
{
    int counter = 0;
    while (counter < length)
    {
        for (int i = 0; i < 3; i++)
        {
            if (buttonPushed(i))
            {
                lightUpLed(i);
                _delay_ms(DELAY);
                lightDownLed(i);

                if (i == pattern[counter])
                {
                    printf("Je drukte op knop %d, correct!\n", i + 1);
                    counter++;
                }
                else
                {
                    printf("Je drukte op knop %d, fout!\n", i + 1);
                    return 0;
                }
                while (buttonPushed(i))
                    ; // wacht tot knop losgelaten is
            }
        }
        _delay_ms(100);
    }
    return 1;
}

//main spel
int main()
{
    initUSART();
    enableAllLeds();
    enableAllButtons();
    enableAllButtonInterrupts();

    printf("Druk op knop 1 om het spel te starten...\n");

    waitForStart(); // led4 knippert, seeden van rand

    uint8_t pattern[MAX_LENGTH];
    generatePuzzle(pattern, MAX_LENGTH);
    printf("Random gegenereerde reeks: ");
    printPuzzle(pattern, MAX_LENGTH); 

    for (int level = 1; level <= MAX_LENGTH; level++)
    {
        printf("\nLevel %d: Speel het patroon na...\n", level);
        playPuzzle(pattern, level);
        if (readInput(pattern, level))
        {
            if (level < MAX_LENGTH)
            {
                printf("Correct, we gaan naar level %d!\n", level + 1);
                flashFeedbackLed(3);
            }
            // Als het laatste level is gehaald, gewoon door naar feliciteren
        }
        else
        {
            printf("Fout! Het juiste patroon was: ");
            printPuzzle(pattern, level);
            for (int i = 0; i < 4; i++)
                lightUpLed(i); // alle leds aan
            while (1)
                ; // stop spel
        }
    }

    printf("Proficiat, je bent de Simon Master!\n");
    flashFeedbackLed(10);
    while (1)
        ; 
          
}
