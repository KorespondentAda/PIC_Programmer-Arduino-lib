#include "Icsp.h"

namespace pic {

Icsp::Icsp(Pin pgm, Pin pgd, Pin pgc, Pin mclr) :
    _pgm(pgm), _pgd(pgd), _pgc(pgc), _mclr(mclr) {}

Word Icsp::Read(int bitCount) {
    setMode(ModeRead);
    clockCycle(bitCount);
    return _buffer;
}

void Icsp::Write(Word data, int bitCount) {
    setMode(ModeWrite);
    _buffer = data;
    _bufBit = 0;
    clockCycle(bitCount);
}

void Icsp::clockCycle() {
    _pgc.Set();
    _mode == ModeWrite ? _buffer |= (digitalRead(_pgd) << _bufBit++) :
                         digitalWrite(_buffer & (1 << _bufBit++));
    _pgc.Reset();
    clockDelay();
}

void Icsp::clockDelay() {
    delayMicroseconds(_clockPeriod);
}

void Icsp::setMode(bool mode) {
    _mode = mode;
    pinMode(_pgd, _mode == ModeWrite ? OUTPUT : INPUT);
}

}
