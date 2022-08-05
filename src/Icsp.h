#pragma once

#include "types.h"

namespace pic {

template <class T>
class Icsp {
public:
    Icsp(int bitCount = 8, Pin pgd = 3, Pin pgc = 4) :
            _bits(bitCount), _pgd(pgd), _pgc(pgc) {}

    T Read() {
        setMode(ModeRead);
        _buffer = 0;
        clockCycle();
        return _buffer;
    }

    T Read(int bitCount) {
        _bits = bitCount;
        return Read();
    }

    void Write(T data) {
        setMode(ModeWrite);
        _buffer = data;
        clockCycle();
    }

    void Write(T data, int bitCount) {
        _bits = bitCount;
        Write(data);
    }

private:
    void setMode(int mode) {
        _mode = mode;
        _pgd.Mode(_mode == ModeWrite ? OUTPUT : INPUT);
    }

    void clockCycle() {
        int bufBit = 0;
        while (bufBit < _bits) {
            _pgc.Set();
            if (_mode == ModeRead)
                _buffer |= (_pgd.Read() << bufBit);
            else
                _pgd.Write(_buffer & (1 << bufBit));
            _pgc.Reset();
            bufBit++;
        }
        _pgd.Reset();
    }

private:
    enum {
        ModeRead = 0,
        ModeWrite = 1
    };

    int _mode;
    int _bits;
    Pin _pgd;
    Pin _pgc;
    T _buffer;
};

}
