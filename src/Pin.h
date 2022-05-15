
#pragma once

namespace pic {

class Pin {
private:
    int _pin;
public:
    Pin(int pin) : _pin(pin) { pinMode(_pin, OUTPUT); digitalWrite(_pin, LOW); }
    void Set() { digitalWrite(_pin, HIGH); }
    void Reset() { digitalWrite(_pin, LOW); }
    bool Read() { return digitalRead(_pin); }
    void Mode(int mode) { pinMode(_pin, mode); }
};

}

