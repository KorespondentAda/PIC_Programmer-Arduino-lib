/* Pic.h - Part of `PIC_Programmer` Arduino library
 * Created by KorespondentAda
 * Released into the public domain
 *
 * Defines class `Pic` that used as common program interface to PIC chips
 */
#pragma once
#include "types.h"
#include "Icsp.h"
#include "Hex.h"

namespace pic {

// TODO: Make `Hexable` members optional, only if lib provided
class Pic : public Hexable {
	Pin _pgm;                 // Programming mode control
	Pin _mclr;                // Programming voltage control
	Word _position;           // Current PC value
	Icsp<Word> _interface;    // Used InCircuit Programming Interface

	Word _memStart;
	Word _memStop;

public:
	Pic(int pgm, int pgc, int pgd, int mclr) :
			_pgm(pgm), _mclr(mclr), _position(0), _interface(pgc, pgd, 0) {};

	// TODO Error checks
	int setMemoryMap(Word addrStart, Word addrStop) {
		_memStart = addrStart;
		_memStop = addrStop;
		return _memStop - _memStart;
	}

	// Begin or end programming cycle
	void begin();
	void end();

	// Read from current poisition
	Word read();

	// Write to current poisition and step next
	void write(Word);

	// Erase cell at current position then write
	void rewrite(Word);

	// Erase chip. mode is:
	//	 0: full
	//	 1: program
	//	 2: data
	void erase(int mode);

	// Move position to specified address
	void seek(Word address);

	// Move to next address
	void step() { seek(_position + 1); }

public: // Hexable
	int GetData([[maybe_unused]]size_t *address, uint8_t buf[], size_t byteCount) override {
		size_t bytes = 0;
		byte b;
		*address = (size_t)_position * 2;
		while (bytes < byteCount) {
			if (_position < _memStart || _position > _memStop)
				return (bytes ? bytes : -1);
			if (bytes % 2 == 0) {
				b = lowByte(read());
			} else {
				b = highByte(read());
				step();
			}
			buf[bytes] = b;
			bytes++;
		}
		return bytes;
	}

private:
	// Find address in memory sections
	bool isProgramMemory() { return isProgramMemory(_position); }
	bool isProgramMemory(Word addr) { return !(addr & 0x2000); }
	bool isDataMemory() { return isDataMemory(_position); }
	bool isDataMemory(Word addr) { return (addr >> 8) == 0x21; }

	// Just send command
	void sendCommand(int cmd) {
		_interface.Write(cmd, 6);
		delayMicroseconds(1);
		delay(1);
	}

	// Send command then read input data
	void sendCommandRead(int cmd, Word &data) {
		sendCommand(cmd);
		data = (_interface.Read(16) >> 1) & 0x3FFF;
		delayMicroseconds(1);
	}

	// Send command then write output data
	void sendCommandWrite(int cmd, Word data) {
		sendCommand(cmd);
		// TODO Clear `& 0x3FFF`
		data = ((data & 0x3FFF) << 1) & 0x7FFE;
		_interface.Write(data, 16);
		delayMicroseconds(1);
	}
};

}
