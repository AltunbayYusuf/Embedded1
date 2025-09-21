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
#include "buzzer.h"

#define LEVEL_MIN 1
#define LEVEL_MAX 10

volatile int gameStart = 0;
volatile int chosenSong = 0;

// pointers naar variabelen zodat we er later gemakkelijk in functies aan kunnen
int *aantalHitsPtr;
int *levelPtr;
int *previousPotPtr;

int *song1Ptr;
int song1Length;

int *song2Ptr;
int song2Length;

int *currentSongPtr;
int currentSongLength;

// interrupt die afgaat als een knop wordt ingedrukt (button 0 = startknop)
ISR(PCINT1_vect)
{
    if (buttonPushed(0)) {
        gameStart = 1;
    }
}

// korte victory tune om af te spelen na een level
void playVictoryTune()
{
    int notes[] = { 523, 659, 783, 1046 };
    int durations[] = { 150, 150, 150, 300 };

    for (int i = 0; i < 4; i++) {
        playTone(notes[i], durations[i]);
        _delay_ms(50);
    }
}

// Liedje 1 tonen (toonfrequentie berekend op basis van index)
void playToneSong1(int note, int duration) {
    int freq = 440 + note * 100;
    playTone(freq, duration);
}

// Liedje 2 tonen (andere berekening voor toonhoogte)
void playToneSong2(int note, int duration) {
    int freq = 330 + note * 120;
    playTone(freq, duration);
}

// toont voorvertoning van het liedje (zodat je weet welke knoppen je moet drukken)
void playPreview(int delayPerNote)
{
    printf("🎧 Preview van het liedje...\n");

    for (int i = 0; i < currentSongLength; i++)
    {
        int note = currentSongPtr[i];

        writeLineTop(note);
        _delay_ms(delayPerNote / 3);

        writeLineMiddle(note);
        _delay_ms(delayPerNote / 3);

        writeLineBottom(note);

        // juiste toon afspelen afhankelijk van gekozen liedje
        if (chosenSong == 0) {
            playToneSong1(note, delayPerNote);
        } else {
            playToneSong2(note, delayPerNote);
        }

        lightUpLed(note);
        _delay_ms(100);
        lightDownLed(note);
    }

    lightDownAllLeds();
    _delay_ms(500);
}

// speelt een noot en kijkt of de speler op tijd op de juiste knop drukt
int playNoteWithCheck(int note, int delayPerNote)
{
    int timeout = delayPerNote;
    int pressed = 0;

    // we tonen de noot op het display in drie stappen
    writeLineTop(note);
    _delay_ms(delayPerNote / 3);

    writeLineMiddle(note);
    _delay_ms(delayPerNote / 3);

    writeLineBottom(note);

    // speler krijgt tijd om te reageren
    for (int i = 0; i < timeout; i++)
    {
        if (buttonPushed(note))
        {
            pressed = 1;
            break;
        }
        _delay_ms(1);
    }

    // speler was op tijd en correct
    if (pressed)
    {
        if (chosenSong == 0) {
            playToneSong1(note, delayPerNote);
        } else {
            playToneSong2(note, delayPerNote);
        }
        _delay_ms(200);
        return 1;
    }
    else
    {
        // fout of te laat gedrukt
        lightDownAllLeds();
        printf("Fout! Je drukte te laat, te vroeg of verkeerd.\n");
        _delay_ms(1000);
        return 0;
    }
}

int main()
{
    // geheugen reserveren voor onze variabelen
    aantalHitsPtr = (int*)malloc(sizeof(int));
    levelPtr = (int*)malloc(sizeof(int));
    previousPotPtr = (int*)malloc(sizeof(int));

    // liedjes definiëren
    int song1[] = {0, 1, 2, 1, 2, 0, 2, 1, 0};
    song1Length = sizeof(song1) / sizeof(int);
    song1Ptr = (int*)malloc(song1Length * sizeof(int));
    for (int i = 0; i < song1Length; i++) {
        song1Ptr[i] = song1[i];
    }

    int song2[] = {2, 2, 1, 0, 1, 0, 2, 1, 0};
    song2Length = sizeof(song2) / sizeof(int);
    song2Ptr = (int*)malloc(song2Length * sizeof(int));
    for (int i = 0; i < song2Length; i++) {
        song2Ptr[i] = song2[i];
    }

    *aantalHitsPtr = 0;
    *levelPtr = 1;
    *previousPotPtr = 0;

    // alles initialiseren
    initUSART();
    initDisplay();
    initPotmeter();
    enableAllButtons();
    enableAllButtonInterrupts();
    enableAllLeds();
    enableBuzzer();
    sei(); // interrupts aanzetten

    // liedje kiezen met knop 0 of 1
    printf("Selecteer liedje (0 voor Liedje 1, 1 voor Liedje 2):\n");
    while (!gameStart)
    {
        if (buttonPushed(0)) {
            chosenSong = 0;
            gameStart = 1;
        }
        if (buttonPushed(1)) {
            chosenSong = 1;
            gameStart = 1;
        }
    }

    // het gekozen liedje instellen
    if (chosenSong == 0)
    {
        currentSongPtr = song1Ptr;
        currentSongLength = song1Length;
        printf("🎵 Liedje 1 geselecteerd.\n");
    }
    else
    {
        currentSongPtr = song2Ptr;
        currentSongLength = song2Length;
        printf("🎵 Liedje 2 geselecteerd.\n");
    }

    // gameStart resetten
    gameStart = 0;

    *previousPotPtr = readPotmeterValue();
    writeNumberToSegment(3, *levelPtr);

    // level instellen met potentiometer
    printf("Draai aan de potentiometer om je startlevel te kiezen.\n");
    printf("Druk op knop 1 om het spel te starten.\n");

    while (!gameStart)
    {
        int currentPot = readPotmeterValue();
        int delta = currentPot - *previousPotPtr;

        if (abs(delta) > 1)
        {
            int step = delta / 50; // gevoeligheid instellen
            if (step == 0)
                step = (delta > 0) ? 1 : -1;

            *levelPtr += step;

            if (*levelPtr < LEVEL_MIN)
                *levelPtr = LEVEL_MIN;
            if (*levelPtr > LEVEL_MAX)
                *levelPtr = LEVEL_MAX;

            *previousPotPtr = currentPot;

            writeNumberToSegment(3, *levelPtr);
            printf("Level: %d\n", *levelPtr);
        }

        _delay_ms(50);
    }

    // de echte game loop
    while (*levelPtr <= LEVEL_MAX)
    {
        int delayPerNote = 500 - (*levelPtr - 1) * 50;
        if (delayPerNote < 100)
            delayPerNote = 100;

        int eersteKeerInLevel = 1;
        int correctePogingen = 0;

        lightDownAllLeds();

        // je moet 4 keer het liedje correct naspelen om het level te halen
        while (correctePogingen < 4)
        {
            writeNumberToSegment(3, *levelPtr);
            printf("\n🎮 Level %d - Correcte pogingen: %d/4\n", *levelPtr, correctePogingen);

            if (eersteKeerInLevel)
            {
                playPreview(delayPerNote);
                eersteKeerInLevel = 0;
            }

            int correct = 1;
            *aantalHitsPtr = 0;

            for (int i = 0; i < currentSongLength; i++)
            {
                if (playNoteWithCheck(currentSongPtr[i], delayPerNote))
                {
                    (*aantalHitsPtr)++;
                    printf("Hits: %d\n", *aantalHitsPtr);
                    writeNumberToSegment(3, *aantalHitsPtr);
                }
                else
                {
                    correct = 0;
                    *aantalHitsPtr = 0;
                    writeNumberToSegment(3, *aantalHitsPtr);
                    printf("❌ Verkeerd! Probeer het nog eens.\n");
                    break;
                }
            }

            if (correct)
            {
                if (correctePogingen < 4)
                {
                    correctePogingen++;
                    for (int i = 0; i < correctePogingen; i++)
                    {
                        lightUpLed(i);
                    }
                    printf("✅ Goed gedaan! %d/4 correcte pogingen.\n", correctePogingen);
                }
                _delay_ms(1000);
            }
            else
            {
                printf("geen stress, je voortgang blijft hoor\n");
            }
        }

        // als 4 pogingen goed zijn -> volgend level
        printf("🏁 Level %d GEHAALD!\n", *levelPtr);

        lightDownAllLeds();
        (*levelPtr)++;
        playVictoryTune();
    }

    // als laatste level gehaald is
    printf("\n🎉 Gefeliciteerd! Je hebt alle levels gehaald!\n");
    lightUpAllLeds();
    _delay_ms(2000);
    lightDownAllLeds();

    // geheugen netjes opruimen
    free(aantalHitsPtr);
    free(levelPtr);
    free(previousPotPtr);
    free(song1Ptr);
    free(song2Ptr);

    return 0;
}
