/* Pic.h - Part of `PIC_Programmer` Arduino library
 * Created by KorespondentAda
 * Released into the public domain
 *
 * Defines class `Pic` that used as common program interface to PIC chips
 */
#include "Pic.h"

namespace pic {

void Pic::begin() {
	_pgm.Set();
	// 100 ns delay, one instruction must be enought
	//_NOP();
	//delayMicroseconds(1);
	_mclr.Set();
	_position = 0;
	delayMicroseconds(5);
}

void Pic::end() {
	_mclr.Reset();
	// 100 ns delay, one instruction must be enought
	//_NOP();
	//delayMicroseconds(1);
	_pgm.Reset();
	delayMicroseconds(5);
}

Word Pic::read() {
	Word result;
	sendCommandRead(0b000100 | (isDataMemory() ? 1 : 0), result);
	return result;
}

void Pic::write(Word w) {
	// Send load command: for Data Memory if currently in data
	sendCommandWrite(0b000010 | (isDataMemory() ? 1 : 0), w);
	// Send programming command
	sendCommand(0b011000); // Begin Programming Only Cycle
	delay(4);
}

void Pic::rewrite(Word w) {
	// Load data word
	sendCommandWrite(0b000010 | (isDataMemory() ? 1 : 0), w);
	sendCommand(0b001000); // Begin Erase Programming Cycle
	delay(8);
}

void Pic::erase(int mode) {
	end();
	begin();
	switch (mode) {
		case 0:
			seek(0x2007);
			sendCommand(0b000001);
			sendCommand(0b000111);
			sendCommand(0b001000);
			delay(50);
			sendCommand(0b000001);
			sendCommand(0b000111);
			break;
		case 1:
			sendCommandWrite(0b000010, 0x3FFF);
			sendCommand(0b000001);
			sendCommand(0b000111);
			sendCommand(0b001000);
			delay(50);
			sendCommand(0b000001);
			sendCommand(0b000111);
			break;
		case 2:
			sendCommandWrite(0b000011, 0x3FFF);
			sendCommand(0b000001);
			sendCommand(0b000111);
			sendCommand(0b001000);
			delay(50);
			sendCommand(0b000001);
			sendCommand(0b000111);
			break;
		default:
			Serial.println("[Pic::erase] ERROR: invalid mode");
	}
	end();
	begin();
}

void Pic::seek(Word address) {
	// Reset two upper bits as unrelated to 14-bit address word
	address &= 0x3FFF;

	// TODO Program Counter changes cyclic,
	// so maybe short ranges should be stepped instead of dropping?
	// It can be faster that way
	if (address < _position) {
		end();
		begin();
	}

	if (!isProgramMemory(address) && isProgramMemory()) {
		sendCommandWrite(0, 0); // Load Configuration command
		_position = 0x2000;
	}

	for (; _position < address; _position++)
		sendCommand(0b000110); // Increment Address command
}

}

