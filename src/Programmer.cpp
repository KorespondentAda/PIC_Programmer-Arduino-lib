/*
 * Programmer.cpp - Library for programming PIC microcontrollers
 * Created by KorespondentAda
 */
#include "Programmer.h"

namespace pic {

// Internal methods

void assert(bool cond, const char mess[]) {
    if (!cond) {
        Serial.print("Error in Programmer.cpp: ");
        Serial.println(mess);
        while (true) _NOP();
    }
}


// Public methods

Programmer::Programmer(int pPgm, int pPgc, int pPgd, int pMclr) :
    _pgm(pPgm), _pgc(pPgc), _pgd(pPgd), _mclr(pMclr), _dev(new Device) { }

void Programmer::Init() {
    Serial.begin(9600);
    Serial.println("Serial started");

    pinMode(_pgm, OUTPUT);
    pinMode(_pgc, OUTPUT);
    pinMode(_pgd, OUTPUT);
    pinMode(_mclr, OUTPUT);
    pinReset(_pgm);
    pinReset(_pgc);
    pinReset(_pgd);
    pinReset(_mclr);
}

void Programmer::ReadConfiguration(bool verbose) {
    startProgramMode();
    enterConfiguration();
    readWord(_dev->idLocations, _dev->idLocationsCount);
    increasePc(2);
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

void Programmer::PrintConfiguration() {
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

    Serial.print("Device ID: ");
    Serial.print(_dev->Id());
    Serial.print(", rev. ");
    Serial.println(_dev->Id() & 0x1F);

    Serial.print("Configuration flags (");
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

void Programmer::EraseProgram() {
    Serial.println("Erasing program memory...");
    sendCommand(Device::Cmd::loadProg);
    sendWord(0x3FFF);
    eraseSequence();
}

void Programmer::EraseData() {
    Serial.println("Erasing data memory...");
    sendCommand(Device::Cmd::loadData);
    sendWord(0x3FFF);
    eraseSequence();
}

void Programmer::EraseChip() {
    if (_dev->isProtected()) {
        Serial.println("Erase protected memory");
        startProgramMode();
        enterConfiguration();
        increasePc(7);
        assert(_dev->Pc() == 0x2007, "Wrong adress");
        eraseSequence();
        stopProgramMode();
    } else {
        EraseProgram();
        EraseData();
    }
    Serial.println("Chip erased");
}

void Programmer::WriteConfiguration() {
    startProgramMode();
    enterConfiguration();
    writeWord(_dev->idLocations, 4, false);
    increasePc(3);
    writeWord(_dev->ConfigurationWord, false);
    stopProgramMode();
}

void Programmer::WriteChip() {
    WriteProgram();
    WriteData();
    WriteConfiguration();
}

void Programmer::RewriteChip() {
    EraseChip();
    WriteChip();
}

void Programmer::SetConfigurationIdLocation(Word idLocation, int number) {
    assert(number < _dev->idLocationsCount, "Error: number of ID Location greater than count");
    _dev->idLocations[number] = idLocation;
}

void Programmer::SetConfigurationWord(Word confWord) {
    _dev->ConfigurationWord = confWord;
}


// Private methods

void Programmer::pinSet(int pin) {
    digitalWrite(pin, HIGH);
}

void Programmer::pinReset(int pin) {
    digitalWrite(pin, LOW);
}

void Programmer::clockDelay() {
    _NOP();     // TODO Replace with calculated timings
}

void Programmer::clockPulse() {
    pinSet(_pgc);
    clockDelay();
    pinReset(_pgc);
    clockDelay();
}

void Programmer::sendBit(bool b) {
    b ? pinSet(_pgd)
      : pinReset(_pgd);
    clockPulse();
}

Word Programmer::readBit() {
    clockPulse();
    return digitalRead(_pgd);
}

void Programmer::sendCommand(Device::Cmd cmd) {
    for (int b = 0; b < _dev->CommandLength; b++)
        sendBit(cmd, b);
    delayMicroseconds(1);
}

void Programmer::sendWord(Word w, bool step, bool conf) {
    bool isData = _dev->isPcInData();
    sendCommand(     conf ? Device::Cmd::loadConf
                 : isData ? Device::Cmd::loadData
                          : Device::Cmd::loadProg);
    sendBit(0);
    for (int b = 0; b < (isData ? _dev->DataWordLength : _dev->ProgramWordLength); b++)
        sendBit(w, b);
    sendBit(0);
    if (step) increasePc();
    delayMicroseconds(1);
}

Word Programmer::readWord(bool step) {
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

void Programmer::readWord(Word w[], int count) {
    for (int i = 0; i < count; i++)
        w[i] = readWord();
}

void Programmer::writeWord(Word payload, bool erase, int retries, bool step) {
    do {
        sendWord(payload, false);
        sendCommand(erase ? Device::Cmd::begEraseProg
                          : Device::Cmd::begProg);
        if (erase) delay(4);
        delay(4);
    } while (retries-- > 0 && readWord(false) != payload);
    if (step) increasePc();
}

void Programmer::writeWord(Word payload[], int count, bool erase, int retries) {
    for (int i = 0; i < count; i++)
        writeWord(payload[i], erase, retries);
}

void Programmer::increasePc(PcSize count) {
    for (int i = 0; i < count; i++) {
        sendCommand(Device::Cmd::pcInc);
    }
    _dev->IncreasePc(count);
}

void Programmer::startProgramMode() {
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

void Programmer::stopProgramMode() {
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

void Programmer::enterConfiguration() {
    sendWord(0x3FFF, false, true);
    _dev->Jump(Device::MemoryBlock::Configuration);
}

int Programmer::serialWriteWord(Word w) {
    return Serial.write(highByte(w)) + Serial.write(lowByte(w));
}

int Programmer::serialWriteWord(Word w[], int count) {
    size_t bytes = 0;
    for (int i = 0; i < count; i++)
        bytes += Serial.write(highByte(*(w + i))) + Serial.write(lowByte(*(w + i)));
    return bytes;
}

void Programmer::eraseSequence() {
    sendCommand(Device::Cmd::bulkErase1);
    sendCommand(Device::Cmd::bulkErase2);
    sendCommand(Device::Cmd::begEraseProg);
    delay(8);
    sendCommand(Device::Cmd::bulkErase1);
    sendCommand(Device::Cmd::bulkErase2);
}

};

