/* Pin.h - Part of `PIC_Programmer` Arduino library
 * Created by KorespondentAda
 * Released into the public domain
 *
 * Class `Pin` is just wrapper around standart arduino methods
 * like `digitalWrite()`.
 */
#pragma once
#include "Arduino.h"

namespace pic {

class Pin {
public:
	Pin(int pin) : _pin(pin) {
		pinMode(_pin, OUTPUT);
		digitalWrite(_pin, LOW);
	}

	void Set() {
		digitalWrite(_pin, HIGH);
	}

	void Reset() {
		digitalWrite(_pin, LOW);
	}

	bool Read() {
		return digitalRead(_pin);
	}

	void Write(bool level) {
		level ? Set() : Reset();
	}

	void Mode(int mode) {
		pinMode(_pin, mode);
	}

private:
	int _pin;
};

}

