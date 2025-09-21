#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "button.h"
#include "display.h"
#include "usart.h"
#include "potentiometer.h"
#include "led.h"

#define START_AANTAL 21
#define MAX_AANTAL 3

volatile int gameStart = 0;
volatile char beurt;
volatile int aantal;
volatile int keuze;
volatile int bevestig = 0;

volatile int teller = 0;

volatile char *beurten;
volatile int *aantallen;
volatile int *beschikbaarAantal;

int computerKeuze(int beschikbaarAantal)
{
    int move = (beschikbaarAantal - 1) % (MAX_AANTAL + 1);
    if (move == 0 || move > beschikbaarAantal)
    {
        move = (rand() % MAX_AANTAL) + 1;
        if (move > beschikbaarAantal)
            move = beschikbaarAantal;
    }
    return move;
}

ISR(PCINT1_vect)
{
    _delay_us(1000);

    if (buttonPushed(0))
    {
        if (!gameStart)
        {
            gameStart = 1;
        }
        else if (beurt == 'P')
        {
            if (keuze > 1)
                keuze--;
        }
    }
    else if (buttonPushed(1))
    {
        if (gameStart)
        {
            if (beurt == 'P')
            {
                beurten[teller] = beurt;
                aantallen[teller] = keuze;
                aantal -= keuze;
                beschikbaarAantal[teller] = aantal;
                teller++;
                beurt = 'C';
                bevestig = 0;
            }
            else if (beurt == 'C')
            {
                if (bevestig == 0)
                {
                    keuze = computerKeuze(aantal);
                    bevestig = 1;
                }
                else if (bevestig == 1)
                {
                    beurten[teller] = beurt;
                    aantallen[teller] = keuze;
                    aantal -= keuze;
                    beschikbaarAantal[teller] = aantal;
                    teller++;
                    beurt = 'P';
                    bevestig = 0;
                }
            }
            if (aantal <= 1)
            {
                gameStart = 0;
            }
        }
    }
    else if (buttonPushed(2))
    {
        if (gameStart && beurt == 'P')
        {
            if (keuze < MAX_AANTAL && keuze < aantal)
                keuze++;
        }
    }
}

void initGame()
{
    aantal = START_AANTAL;
    keuze = MAX_AANTAL;
    bevestig = 0;
    teller = 0;

    beurten = calloc(START_AANTAL, sizeof(char));
    aantallen = calloc(START_AANTAL, sizeof(int));
    beschikbaarAantal = calloc(START_AANTAL, sizeof(int));
}

int main()
{
    initUSART();
    initDisplay();
    initPotmeter();
    enableAllButtons();
    enableAllButtonInterrupts();
    enableAllLeds();

    initGame();

    int shown = 0;
    while (!gameStart)
    {
        if (!shown)
        {
            printf("Welkom bij het Luciferspel!\n");
            printf("Draai aan de potentiometer om een willekeurige seed te genereren.\n");
            printf("Druk op knop 1 om het spel te starten.\n");
            shown = 1;
        }
        int rawValue = readPotmeterValue();
        int seed = (rawValue * 10000L) / 1024;
        writeNumber(seed);
        srand(seed);
    }

    beurt = (rand() % 2) ? 'P' : 'C';
    printf("Het spel start! %s begint.\n", (beurt == 'P') ? "Speler" : "Computer");

    while (gameStart)
    {
        if (aantal <= 1)
        {
            gameStart = 0;
            break;
        }

        if (beurt == 'C' && bevestig == 0)
        {
            // computer maakt de keuze
        }

        for (int i = 0; i < 1000; i++)
        {
            if (i < 500)
            {
                writeCharToSegment(1, beurt);
            }
            else
            {
                writeCharToSegment(1, ' ');
            }
            writeNumberToSegment(0, keuze);
            writeNumberToSegment(2, aantal / 10);
            writeNumberToSegment(3, aantal % 10);
            _delay_ms(1);
        }
    }

    for (int i = 0; i < teller; i++)
    {
        printf("%c nam op beurt %2d %d lucifer(s), waardoor er %2d lucifers overbleven.\n",
               beurten[i], i + 1, aantallen[i], beschikbaarAantal[i]);
    }

    char winnaar = (beurt == 'P') ? 'C' : 'P';

    while (1)
    {
        if (winnaar == 'P')
        {
            writeStringAndWait("WIN", 1000);
        }
        else
        {
            writeStringAndWait("LOSE", 1000);
        }
        writeStringAndWait("YOU", 1000);
    }

    free(beurten);
    free(aantallen);
    free(beschikbaarAantal);

    return 0;
}
