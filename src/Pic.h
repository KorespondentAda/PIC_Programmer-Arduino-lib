/* Pic.h - Part of `PIC_Programmer` Arduino library
 * Created by KorespondentAda
 * Released into the public domain
 *
 * Defines class `Pic` that used as common program interface to PIC chips
 */
#pragma once
#include "types.h"
#include "Icsp.h"

namespace pic {

class Pic {
	Pin _pgm;
	Pin _mclr;
	Word _position;
	Icsp<Word> _interface;

public:
	Pic(int pgm, int pgc, int pgd, int mclr) :
			_pgm(pgm), _mclr(mclr), _position(0), _interface(pgc, pgd, 0) {};

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
