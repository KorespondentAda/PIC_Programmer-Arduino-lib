/*
 * Programmer.cpp - Library for programming PIC microcontrollers
 * Created by KorespondentAda
 */
#include "Programmer.h"

namespace pic {

// Internal methods

void assert(bool cond) {
    if (!cond) {
        Serial.println();
        Serial.println("Runtime error in Programmer.cpp. Halt.");
        while (true) _NOP();
    }
}

void assert(bool cond, const char mess[]) {
    if (!cond) {
        Serial.println();
        Serial.print("Error in Programmer.cpp: ");
        Serial.println(mess);
        while (true) _NOP();
    }
}


// Public methods

Programmer::Programmer(int pPgm, int pPgc, int pPgd, int pMclr) :
    _pgm(pPgm), _pgc(pPgc), _pgd(pPgd), _mclr(pMclr), _dev(new Device), _hexBufferSize(0x10) { }

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

void Programmer::ReadConfiguration() {
    startProgramMode();
    enterConfiguration();
    readWord(_dev->idLocations, _dev->idLocationsCount);
    increasePc(2);
    _dev->Id(readWord());
    _dev->ConfigurationWord = readWord();
    stopProgramMode();
}

void Programmer::ReadProgram(int count) {
    if (count == 0) {
        count = _dev->ProgramMemoryStop - _dev->ProgramMemoryStart;
    }
    Word buffer[_hexBufferSize / 2];
    uint8_t recordSize, checksum;
    startProgramMode();
    while (count > 0) {
        if (count * 2 > _hexBufferSize) {
            recordSize = _hexBufferSize;
        } else {
            recordSize = count * 2;
        }
        checksum = 0;
        // Start record
        assert(Serial.print(":") == 1);
        // Record data size
        assert(serialPrintByte(recordSize) == 2);
        checksum += recordSize;
        // Record start address
        assert(serialPrintWord(_dev->Pc() * 2, false) == 4);
        checksum += highByte(_dev->Pc() * 2);
        checksum +=  lowByte(_dev->Pc() * 2);
        // Record data type - default
        assert(serialPrintByte(0) == 2);
        // Record data - read & print
        readWord(buffer, recordSize / 2);
        assert(serialPrintWord(buffer, recordSize / 2) == 2*recordSize);
        for (int i = 0; i < recordSize / 2; i++) {
            checksum += highByte(buffer[i]);
            checksum +=  lowByte(buffer[i]);
        }
        // Checksum calculate - two's complement of record bytes sum
        checksum = (~checksum) + 1;
        assert(serialPrintByte(checksum) == 2);
        Serial.println();
        count -= recordSize / 2;
    }
    stopProgramMode();
    // EOF sequence
    Serial.println(":00000001FF");
    Serial.println();
    Serial.println("Program readed");
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
    startProgramMode();
    sendWord(0x3FFF);
    eraseSequence();
    stopProgramMode();
}

void Programmer::EraseData() {
    Serial.println("Erasing data memory...");
    startProgramMode();
    enterConfiguration();
    increasePc(0x0100);
    sendWord(0x3FFF);
    eraseSequence();
    stopProgramMode();
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

void Programmer::WriteProgram() {
    uint8_t recordSize;
    uint8_t recordType;
    uint8_t checksum = 0;
    Word recordAddress;
    Word buff[_hexBufferSize];
    Serial.println("Waiting for program input");
    while (Serial.available() == 0);
    startProgramMode();
    while (true) {
        while (Serial.read() != ':');
        checksum = recordSize = serialReadByte();
        Serial.print("Record size: ");
        Serial.println(recordSize);
        Serial.print("Buffer size: ");
        Serial.println(_hexBufferSize);
        Serial.print("Now checksum is: ");
        Serial.println(checksum, HEX);
        assert(recordSize % 2 == 0, "Bytes in record not aligned by words");
        assert(recordSize <= _hexBufferSize, "Record data size larger than buffer size");
        recordAddress = serialReadWord(false);
        checksum += highByte(recordAddress);
        checksum += lowByte(recordAddress);
        Serial.print("Record address: ");
        Serial.println(recordAddress);
        Serial.print("Now checksum is: ");
        Serial.println(checksum, HEX);
        checksum += recordType = serialReadByte();
        Serial.print("Record type: ");
        Serial.println(recordType);
        Serial.print("Now checksum is: ");
        Serial.println(checksum, HEX);
        for (int i = 0; i < recordSize / 2; i++) {
            buff[i] = serialReadWord();
            checksum += highByte(buff[i]) + lowByte(buff[i]);
            Serial.print("Now checksum is: ");
            Serial.println(checksum, HEX);
        }
        Serial.println("Data readed");
        checksum += serialReadByte();
        Serial.print("Resulting checksum: ");
        Serial.println(checksum, HEX);
        assert(checksum == 0, "Bad checksum");
        if (recordSize == 0 && recordAddress == 0 && recordType == 1)
            break;
        else {
            setPc(recordAddress / 2);
            writeWord(buff, recordSize / 2, false);
        }
    }
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

bool Programmer::readBit() {
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

void Programmer::setPc(PcSize address) {
    if (_dev->isPcInProgram())
        if (_dev->isAddressInProgram(address)) {
            if (_dev->Pc() > address) {
                stopProgramMode();
                startProgramMode();
            }
        } else
            enterConfiguration();
    else if (_dev->isAddressInProgram(address)) {
        stopProgramMode();
        startProgramMode();
    }
    increasePc(address - _dev->Pc());
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

void Programmer::eraseSequence() {
    sendCommand(Device::Cmd::bulkErase1);
    sendCommand(Device::Cmd::bulkErase2);
    sendCommand(Device::Cmd::begEraseProg);
    delay(8);
    sendCommand(Device::Cmd::bulkErase1);
    sendCommand(Device::Cmd::bulkErase2);
}

uint8_t Programmer::hex2byte(char high, char low) {
    uint8_t res;
    Serial.print("Converting hex ");
    Serial.print(high);
    Serial.print(low);
    if (high >= '0' && high <= '9') {
        res = (high - '0') << 4;
    } else if (high >= 'A' && high <= 'F') {
        res = (high - 'A' + 10) << 4;
    } else if (high >= 'a' && high <= 'f') {
        res = (high - 'a' + 10) << 4;
    } else {
    assert(false, "High symbol is invalid");
    }
    if (low >= '0' && low <= '9') {
        res += (low - '0');
    } else if (low >= 'A' && low <= 'F') {
        res += (low - 'A' + 10);
    } else if (low >= 'a' && low <= 'f') {
        res += (low - 'a' + 10);
    } else {
    assert(false, "Low symbol is invalid");
    }
    Serial.print(" to byte ");
    Serial.println(res);
    return res;
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

int Programmer::serialPrintByte(uint8_t byte) {
    int printed = 0;
    if (byte < 0x10)
        printed += Serial.print("0");
    printed += Serial.print(byte, HEX);
    return printed;
}

int Programmer::serialPrintWord(Word w, bool le) {
    int printed = 0;
    uint8_t b;
    b = le ? lowByte(w) : highByte(w);
    printed += serialPrintByte(b);
    b = le ? highByte(w) : lowByte(w);
    printed += serialPrintByte(b);
    return printed;
}

int Programmer::serialPrintWord(Word w[], int count, bool le) {
    int printed = 0;
    for (int i = 0; i < count; i++) {
        printed += serialPrintWord(w[i], le);
    }
    return printed;
}

uint8_t Programmer::serialReadByte() {
    while (Serial.available() < 2);
    char h = Serial.read();
    char l = Serial.read();
    return hex2byte(h, l);
}

Word Programmer::serialReadWord(bool le) {
    Word res;
    res  = serialReadByte() << (le ? 0 : 8);
    res += serialReadByte() << (le ? 8 : 0);
    static_assert(uint8_t(0xFF) << 8 == 0xFF00);
    return res;
}

};

