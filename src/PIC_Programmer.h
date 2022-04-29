// vim:set foldmethod=marker ft=arduino:
/*
 * PIC_Programmer.h - Library for programming PIC microcontrollers
 * Created by KorespondentAda
 */
#ifndef __PIC_PROGRAMMER_H__
#define __PIC_PROGRAMMER_H__

#include "Arduino.h"
#include "Device.h"

class PIC_Programmer {
public:
    // Set pins to work with chip through
    PIC_Programmer(int, int, int, int);

    // Interface
    void Init(Device);
    void ReadConfiguration();
    void ReadProgram();
    void ReadData();
    void ReadChip();
    void CleanChip();
    void WriteConfiguration();
    void WriteProgram();
    void WriteData();
    void WriteChip();
    void RewriteChip();

    // Helpers
    void PrintConfiguration();
private:
    int _pgm;                       // Pin ProGram Mode     XXX PIC_23
    int _pgc;                       // Pin ProGram Clock    XXX PIC_27
    int _pgd;                       // Pin ProGram Data     XXX PIC_28
    int _mclr;                      // Pin Not-MCLR         XXX PIC_1
                                    // Power (Vdd)          XXX PIC_20
                                    // Ground (Vss)         XXX PIC_8 | PIC_19
    Device _dev;
};

#endif // __PIC_PROGRAMMER_H__
