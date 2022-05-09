#include "Device.h"

namespace pic {

Device::Device() {
    ProgramWordLength = 14;
    DataWordLength = 8;
    CommandLength = 6;
    ProgramMemoryStart = 0x0000;
    ProgramMemoryStop = 0x1FFF;
    DataMemoryStart = 0x2100;
    DataMemoryStop = 0x2140;
    ConfigurationWord = 0xFFFF;
    idLocationsCount = 4;
    idLocations[0] = 0xFFFF;
    idLocations[1] = 0xFFFF;
    idLocations[2] = 0xFFFF;
    idLocations[3] = 0xFFFF;
    _devId = 2400;
    _pc = 0;
}

Device::Device(const Device& b) {
    _devId = b._devId;
    _pc = b._pc;
    ProgramWordLength = b.ProgramWordLength;
    DataWordLength = b.DataWordLength;
    ConfigurationWord = b.ConfigurationWord;
    idLocationsCount = b.idLocationsCount;
}

void Device::IncreasePc(PcSize count) {
    if (_pc >= 0x2000 - count) _pc = (_pc + count) % 0x2000;
    else if (_pc == 0x3FFF) _pc = ((_pc + count) % 0x2000) + 0x2000;
    else _pc += count;
}

};

