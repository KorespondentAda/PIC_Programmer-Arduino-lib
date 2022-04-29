// vim:set foldmethod=marker ft=arduino:
#include "Device.h"

// Device {{{1
// Public functions {{{2
// Constructors {{{3
Device::Device() : _devId(0) { }
Device::Device(Word devId) : _devId(devId) { }

