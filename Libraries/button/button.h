#include <avr/io.h>
#include <stdio.h>

void enableButton(int button);
void enableAllButtons();
int buttonPushed(int button);
int buttonReleased(int button);

void enableButtonInterrupt(int button);
void enableAllButtonInterrupts();