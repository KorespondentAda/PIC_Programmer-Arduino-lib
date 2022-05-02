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
    _pgm(pPgm), _pgc(pPgc), _pgd(pPgd), _mclr(pMclr), _dev(new Device) {
}

// Init(Device) {{{3
void PIC_Programmer::Init() {
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
}

// ReadConfiguration(bool) {{{3
void PIC_Programmer::ReadConfiguration(bool verbose) {
    startProgramMode();
    enterConfiguration();
    readWord(_dev->idLocations, _dev->idLocationsCount);
    increasePc();
    increasePc();
    _dev->Id(readWord());
    _dev->ConfigurationWord = readWord();
    stopProgramMode();
    if (verbose) {
        serialWriteWord(_dev->idLocations, _dev->idLocationsCount);
        serialWriteWord(0, 2);
        serialWriteWord(_dev->Id());
        serialWriteWord(_dev->ConfigurationWord);
    }
}

// PrintConfiguration() {{{3
void PIC_Programmer::PrintConfiguration() {
    Serial.println();
    Serial.println("Chip configuration:");
    Serial.print("Location ID1: ");
    Serial.println(_dev->idLocations[0]);
    Serial.print("Location ID2: ");
    Serial.println(_dev->idLocations[1]);
    Serial.print("Location ID3: ");
    Serial.println(_dev->idLocations[2]);
    Serial.print("Location ID4: ");
    Serial.println(_dev->idLocations[3]);

    // TODO rewrite this part
    Serial.print("Device ID: ");
    Serial.print(_dev->Id());
    Serial.print(", rev. ");
    Serial.println(_dev->Id() & 0x1F);

    // TODO Parse config word to readable parameters
    Serial.print("Configuration flags (");
    // TODO rewrite typecast; make chip_conf as child of word
    Serial.print(_dev->ConfigurationWord);
    Serial.println("): ");
    Serial.print(_dev->ConfigurationFlags.cp == 3 ? "Code not protected" : "Code protected");
    Serial.print(" (");
    Serial.print(_dev->ConfigurationFlags.cp);
    Serial.println(")");
    Serial.println(_dev->ConfigurationFlags.cpd ? "Data not protected" : "Data protected");
    Serial.println(_dev->ConfigurationFlags.wrt ? "Program write enabled" : "Program write disabled");
    Serial.println(_dev->ConfigurationFlags.lvp ? "Low Voltage Programming enabled" : "Low Voltage Programming disabled");
    Serial.println(_dev->ConfigurationFlags.pwrte ? "Power-Up timer disabled" : "Power-Up timer enabled");
    Serial.println(_dev->ConfigurationFlags.wdte ? "Watchdog timer enabled" : "Watchdog timer disabled");
    Serial.print("Oscillator: ");
    Serial.println(_dev->ConfigurationFlags.osc == 3 ? "RC" : _dev->ConfigurationFlags.osc == 2 ? "HS" : _dev->ConfigurationFlags.osc == 1 ? "XT" : "LP");
    Serial.flush();
}

void assert(bool cond, const char mess[]) {
    if (!cond) {
        Serial.print("Error in PIC_Programmer.cpp: ");
        Serial.println(mess);
        while (true) _NOP();
    }
}

void PIC_Programmer::EraseProgram() {
    assert(false, "EraseProgram: Not implemented");
    sendCommand(Device::Cmd::loadProg);
    sendWord(0x3FFF);
    eraseSequence();
}

void PIC_Programmer::EraseData() {
    assert(false, "EraseData: Not implemented");
    sendCommand(Device::Cmd::loadData);
    sendWord(0x3FFF);
    eraseSequence();
}

void PIC_Programmer::EraseChip() {
    if (_dev->isProtected()) {
        Serial.println("Erase protected memory");
        assert(_dev->ConfigurationFlags.cp == 3, "Хуй залупа");
        startProgramMode();
        enterConfiguration();
        increasePc(7);
        assert (_dev->Pc() == 0x2007, "Wrong adress");
        //sendCommand(Device::Cmd::loadProg);
        //sendWord(0x3FFF, false);
        eraseSequence();
        stopProgramMode();
        startProgramMode();
        enterConfiguration();
        increasePc(7);
        //Serial.println(readWord(false));
        //sendCommand(Device::Cmd::loadProg);
        //sendWord(0x3FFF, false);
        stopProgramMode();
    } else {
        EraseProgram();
        EraseData();
    }
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

// clockDelay() {{{3
void PIC_Programmer::clockDelay() {
    _NOP();     // TODO Replace with calculated timings
}

// clockPulse() {{{3
void PIC_Programmer::clockPulse() {
    pinSet(_pgc);
    clockDelay();
    pinReset(_pgc);
    clockDelay();
}

// sendBit(bool) {{{3
void PIC_Programmer::sendBit(bool b) {
    b ? pinSet(_pgd)
      : pinReset(_pgd);
    clockPulse();
}

// readBit() {{{3
Word PIC_Programmer::readBit() {
    clockPulse();
    return digitalRead(_pgd);
}

// sendCommand(Device::Cmd) {{{3
void PIC_Programmer::sendCommand(Device::Cmd cmd) {
    for (int b = 0; b < _dev->CommandLength; b++)
        sendBit(cmd, b);
    delayMicroseconds(1);
}

// sendWord(Word, bool) {{{3
void PIC_Programmer::sendWord(Word w, bool step) {
    bool isData = _dev->isPcInData();
    sendCommand(isData ? Device::Cmd::loadData
                       : Device::Cmd::loadProg);
    sendBit(0);
    for (int b = 0; b < _dev->ProgramWordLength; b++)
        sendBit(w, b);
    sendBit(0);
    if (step) increasePc();
    delayMicroseconds(2);
}

// readWord() {{{3
Word PIC_Programmer::readWord(bool step) {
    Word w = 0;
    bool isData = _dev->isPcInData();
    sendCommand(isData ? Device::Cmd::readData
                       : Device::Cmd::readProg);
    sendBit(0);
    pinMode(_pgd, INPUT);
    for (int b = 0; b < _dev->ProgramWordLength; b++)
        w |= readBit() << b;
    pinMode(_pgd, OUTPUT);
    sendBit(0);
    delayMicroseconds(1);
    if (step) increasePc();
    return w;
}

// readWord(Word[], int) {{{3
void PIC_Programmer::readWord(Word w[], int count) {
    for (int i = 0; i < count; i++)
        w[i] = readWord();
}

// increasePc(PcSize) {{{3
void PIC_Programmer::increasePc(PcSize count) {
    for (int i = 0; i < count; i++) {
        sendCommand(Device::Cmd::pcInc);
    }
    _dev->IncreasePc(count);
}

// startProgramMode() {{{3
void PIC_Programmer::startProgramMode() {
    pinReset(_pgc);
    pinReset(_pgd);
    pinSet(_pgm);
    // TODO 100ns Delay
    delayMicroseconds(1);       // TODO Device.programModeDelay
    pinSet(_mclr);
    // TODO 100ns Delay
    delayMicroseconds(1);       // TODO Device.programModeDelay
    delayMicroseconds(5);       // TODO Device.programModeDelay
    _dev->ResetPc();
}

// stopProgramMode() {{{3
void PIC_Programmer::stopProgramMode() {
    pinReset(_pgc);
    pinReset(_pgd);
    pinReset(_mclr);
    // TODO 100ns Delay
    delayMicroseconds(1);
    pinReset(_pgm);
    // TODO 100ns Delay
    delayMicroseconds(1);
    delayMicroseconds(5);       // TODO Device.programModeDelay
    _dev->Jump(Device::MemoryBlock::Exit);
}

// enterConfiguration() {{{3
void PIC_Programmer::enterConfiguration() {
    sendCommand(Device::Cmd::loadConf);
    sendWord(0x3FFF, false);
    _dev->Jump(Device::MemoryBlock::Configuration);
}

// serialWriteWord(Word) {{{3
int PIC_Programmer::serialWriteWord(Word w) {
    return Serial.write(highByte(w)) + Serial.write(lowByte(w));
}

// serialWriteWord(Word*, int) {{{3
int PIC_Programmer::serialWriteWord(Word w[], int count) {
    size_t bytes = 0;
    for (int i = 0; i < count; i++)
        bytes += Serial.write(highByte(*(w + i))) + Serial.write(lowByte(*(w + i)));
    return bytes;
}

void PIC_Programmer::eraseSequence() {
    sendCommand(Device::Cmd::bulkErase1);
    sendCommand(Device::Cmd::bulkErase2);
    sendCommand(Device::Cmd::begEraseProg);
    delay(8);
    sendCommand(Device::Cmd::bulkErase1);
    sendCommand(Device::Cmd::bulkErase2);
}
