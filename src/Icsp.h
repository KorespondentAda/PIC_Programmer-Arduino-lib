/* Icsp.h - Part of `PIC_Programmer` Arduino library
 * Created by KorespondentAda
 * Released into the public domain
 *
 * Defines `Icsp` class that is software ICSP realization.
 * Library needs it because of 6-bit commands transaction.
 */
#pragma once

namespace pic {

template <class T>
class Icsp {
public:
	Icsp(Pin pgc = 3, Pin pgd = 4, int bitCount = 8) :
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
		for (int bufBit = 0; bufBit < _bits; bufBit++) {
			if (_mode == ModeWrite)
				_pgd.Write(_buffer & (1 << bufBit));
			_pgc.Set();
			delayMicroseconds(1);
			if (_mode == ModeRead)
				_buffer |= (int(_pgd.Read()) << bufBit);
			_pgc.Reset();
			delayMicroseconds(1);
		}
		_pgd.Reset();
		delayMicroseconds(1);
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
