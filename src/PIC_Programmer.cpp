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

// Init(Device dev) {{{3
void PIC_Programmer::Init(Device dev) {
    // Init serial port for communication
    Serial.begin(9600);
    Serial.println("Serial started");

    Serial.print("[PIC_Programmer::Init] Initialize for device with ID ");
    Serial.println(dev.Id());
    Serial.println("[PIC_Programmer::Init] Stop");
}

// Private functions {{{2

