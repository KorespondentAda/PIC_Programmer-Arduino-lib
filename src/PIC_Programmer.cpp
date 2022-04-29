// vim:set foldmethod=marker ft=arduino:
/*
 * PIC_Programmer.cpp - Library for programming PIC microcontrollers
 * Created by KorespondentAda
 */
#include "PIC_Programmer.h"

// PIC_Programmer {{{1
// Public functions {{{2
// Constructor {{{3
PIC_Programmer::PIC_Programmer(int pPgm, int pPgc, int pPgd, int pMclr) :
    _pgm(pPgm), _pgc(pPgc), _pgd(pPgd), _mclr(pMclr) { }

// Init(Device) {{{3
void PIC_Programmer::Init(Device dev) {
    // Init serial port for communication
    Serial.begin(9600);
    Serial.println("Serial started");

    // Set up pins
    pinMode(_pgm, OUTPUT);
    pinMode(_pgc, OUTPUT);
    pinMode(_pgd, OUTPUT);
    pinMode(_mclr, OUTPUT);
    pinReset(_pgm);
    pinReset(_pgc);
    pinReset(_pgd);
    pinReset(_mclr);

    _dev = dev;
}

// Private functions {{{2
// pinSet(int) {{{3
void PIC_Programmer::pinSet(int pin) {
    digitalWrite(pin, HIGH);
}

// pinReset(int) {{{3
void PIC_Programmer::pinReset(int pin) {
    digitalWrite(pin, LOW);
}

