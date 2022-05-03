/*
 * Programmer.h - Library for programming PIC microcontrollers
 * Created by KorespondentAda
 */
#pragma once

#include "Arduino.h"
#include "Device.h"

namespace pic {

class Programmer {
public:
    // Set pins to work with chip through
    Programmer(int, int, int, int);

    // Interface
    void Init();
    void ReadConfiguration(bool = true);
    void ReadProgram();
    void ReadData();
    void ReadChip();
    void EraseProgram();
    void EraseData();
    void EraseChip();
    void WriteConfiguration();
    void WriteProgram();
    void WriteData();
    void WriteChip();
    void RewriteChip();

    // Helpers
    void PrintConfiguration();
    void SetConfigurationIdLocation(Word idLocation, int number);
    void SetConfigurationWord(Word confWord);

private:
    int _pgm;                       // Pin ProGram Mode     XXX PIC_23
    int _pgc;                       // Pin ProGram Clock    XXX PIC_27
    int _pgd;                       // Pin ProGram Data     XXX PIC_28
    int _mclr;                      // Pin Not-MCLR         XXX PIC_1
                                    // Power (Vdd)          XXX PIC_20
                                    // Ground (Vss)         XXX PIC_8 | PIC_19

    Device *_dev;                   // Device description

    void pinSet(int);               // Set digital pin to High voltage
    void pinReset(int);             // Reset digital pin to Low voltage

    void clockDelay();
    void clockPulse();
    void sendBit(bool);
    void sendBit(Word w, int b) { sendBit(bitRead(w, b)); }
    Word readBit();
    void sendCommand(Device::Cmd);
    void sendWord(Word, bool = true, bool = false);
    Word readWord(bool = true);
    void readWord(Word[], int);
    void writeWord(Word, bool = true, int = 3, bool = true);
    void writeWord(Word[], int, bool = true, int = 3);

    void increasePc(PcSize = 1);

    void startProgramMode();        // Enter programming mode on chip
    void stopProgramMode();         // Exit chip from programming mode
    void enterConfiguration();      // Enter configuration memory space

    void eraseSequence();

    int serialWriteWord(Word);
    int serialWriteWord(Word*, int = 1);
};

};
