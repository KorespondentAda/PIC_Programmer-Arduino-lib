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
#define MaxRecSize 256
#define DefRecSize 16

namespace pic {

// Interface class that allows to be used in `Hex()` for pretty-printed binary
class Hexable {
public:
	// Read `byteCount` bytes to `buf` and save first byte address to `address`
	// Return number of bytes written to buffer
	// Return 0 to skip location and look next
	// Return -1 to finish
	virtual int GetData(size_t *address, uint8_t buf[], size_t byteCount) = 0;
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
		size_t addr;
		unsigned char checksum;

		/* Hex record format:
		 * 	:   - Start of record
		 * 	%2x - Record length, bytes
		 * 	%4x - Record start address, bytes
		 * 	%2x - Record type
		 * 	%s  - Record data
		 * 	%2x - Checksum
		 */
		while ((bytes = _data.GetData(&addr, buf, _recSize)) >= 0) {
			if (bytes == 0) continue;
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
