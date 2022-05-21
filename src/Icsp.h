#pragma once

#include "types.h"

namespace pic {

class Icsp {
public:
    Icsp(Pin = 2, Pin = 3, Pin = 4, Pin = 8);

    Word Read(int bitCount);
    void Write(Word data, int bitCount);

private:
    inline void setMode(bool);

    inline void clockCycle(int);
    inline void clockDelay();

private:
    enum {
        ModeRead = false,
        ModeWrite = true
    };

    Pin _pgm;
    Pin _pgd;
    Pin _pgc;
    Pin _mclr;
    int _clockPeriod;
    Word _buffer;
    int _bufBit;
    bool _mode;

};

}
