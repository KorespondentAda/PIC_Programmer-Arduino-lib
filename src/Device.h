// vim:set foldmethod=marker ft=arduino:
#pragma once

using Word = short;
static_assert(sizeof(Word) == 2);
using PcSize = int;

class Device {
public:
    Device();
    Device(const Device&);

    // TODO Testing initialization, move to child class
    enum Cmd : Word {
         loadConf       = 0x00,
         loadProg       = 0x02,
         readProg       = 0x04,
         pcInc          = 0x06,
         begEraseProg   = 0x08,
         begProg        = 0x18,
         loadData       = 0x03,
         readData       = 0x05,
         bulkErase1     = 0x01,
         bulkErase2     = 0x07
    };
    int ProgramWordLength;
    int DataWordLength;
    int CommandLength;

    struct Flags {
        unsigned osc    : 2;    // bits 0-1; oscillator select:
                                // 00 - LP
                                // 01 - XT
                                // 10 - HS
                                // 11 - RC - internal RC osc
        unsigned wdte   : 1;    // bits 2; watchdog timer enable
        unsigned pwrte  : 1;    // bits 3; power-up timer enable (inverted)
        unsigned cp     : 2;    // bits 4-5; code protection:
                                // 00 - from 0 to end,
                                // 01 - from 1/3 to end,
                                // 10 - from 2/3 to end,
                                // 11 - protection off
        unsigned boden  : 1;    // bits 6; brown-out reset enable
        unsigned lvp    : 1;    // bits 7; low voltage icsp programming enable
        unsigned cpd    : 1;    // bits 8; data EE memory protection (inverted)
        unsigned wrt    : 1;    // bits 9; program memory write enable
        unsigned unim   : 1;    // bits 10; unimplemented, read as 1
        unsigned res    : 1;    // bits 11; reserved, set 1 to be okay
        unsigned acp    : 2;    // bits 12-13; must be same as cp bits
        unsigned sz     : 2;    // bits 14-15, for 16 bit size
    };

    static_assert(sizeof(Flags) == sizeof(Word));
    union {
        Word ConfigurationWord;
        Flags ConfigurationFlags;
    };

    Word Id() { return _devId; }
    Word Id(Word id) { return _devId = id; }
    Word idLocations[4];
    int idLocationsCount;

    enum class MemoryBlock : PcSize {
        Program         = 0x0000,
        Configuration   = 0x2000,
        Data            = 0x2100,
        Exit            = 0x7FFF,
    };

    PcSize Pc() const { return _pc; }
    void ResetPc() { _pc = 0; }
    void IncreasePc(PcSize count = 1) { _pc += count; }
    void Jump(MemoryBlock mb) { _pc = static_cast<PcSize>(mb); }
    bool isPcInData() { return _pc >= 0x2100; }
    bool isProtected() { return (ConfigurationFlags.cp < 3) || (ConfigurationFlags.cpd == 0); }

private:
    Word _devId;
    PcSize _pc;

};
