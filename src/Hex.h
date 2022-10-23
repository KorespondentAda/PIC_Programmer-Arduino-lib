/* Hex.h - Part of `PIC_Programmer` Arduino library
 * Created by KorespondentAda
 * Released into the public domain
 *
 * Defines `Hex` class that is realization of binary data encoding
 * for transmission between systems. Using IntelHEX encoding.
 * Library needs it because of MicroChip datasheet says that programmer need
 * to be able to read or write data to/from hex format.
 */
#pragma once
#include "Print.h"
#include "Printable.h"
// TODO Rename/remove
#define MaxRecSize 256
#define DefRecSize 16

namespace pic {

// Interface class that allows to be used in `Hex()` for pretty-printed binary
class Hexable {
public:
	// Used for printing start address of each record
	// Need to return address of byte that would be readed next
	// Return 0 to skip location and look next
	// Return -1 to finish
	virtual int GetAddress() = 0;

	// Read `byteCount` bytes to `buf`
	// Return number of bytes written to buffer
	virtual int GetData(unsigned char buf[], int byteCount) = 0;
};

class Hex : public Printable {
public:
	Hex(Hexable &d, size_t recordSize) :
			_data(d),
			_recSize(recordSize > MaxRecSize ? MaxRecSize : recordSize)
	{ }
	Hex(Hexable &d) : Hex(d, DefRecSize)
	{ }

	size_t printTo(Print& p) const {
		size_t count = 0;
		unsigned char buf[MaxRecSize];
		int bytes;
		int addr;
		unsigned char checksum;

		/* Hex record format:
		 * 	:   - Start of record
		 * 	%2x - Record length, bytes
		 * 	%4x - Record start address, bytes
		 * 	%2x - Record type
		 * 	%s  - Record data
		 * 	%2x - Checksum
		 */
		while ((addr = _data.GetAddress()) >= 0) {
			// Wait until find data that can be printed
			while ((bytes = _data.GetData(buf, _recSize)) == 0);
			count += p.print(':');
			count += printNum(p, bytes, 2);
			count += printNum(p, addr, 4);
			count += p.print("00");
			checksum = _recSize + (addr >> 8) + (addr & 0xFF);
			for (int i = 0; i < bytes; i++) {
				count += printNum(p, buf[i], 2);
				checksum += buf[i];
			}
			count += printNum(p, 0xFF - checksum + 1, 2);
			count += p.println();
		}

		// "EoF" record
		count += p.print(":00000001FF");

		return count;
	}

private:
	Hexable &_data;
	size_t _recSize;

	static size_t printNum(Print &p, unsigned int num, unsigned int size) {
		size_t count = 0;
		unsigned int mask = 1 << (4 * (size-1));
		while (num < mask && mask > 1) {
			count += p.print('0');
			mask >>= 4;
		}
		count += p.print(num, HEX);
		return count;
	}
};

}
