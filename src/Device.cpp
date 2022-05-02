// vim:set foldmethod=marker ft=arduino:
#include "Device.h"

Device::Device() {
    ProgramWordLength = 14;
    DataWordLength = 8;
    CommandLength = 6;
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

