// vim:set foldmethod=marker ft=arduino:
#ifndef __PIC_PROGRAMMER_DEVICE_H__
#define __PIC_PROGRAMMER_DEVICE_H__

using Word = char16_t;

class Device {
public:
    Device();
    Device(Word);

    Word Id() { return _devId; }

private:
    Word _devId;
};

#endif // __PIC_PROGRAMMER_DEVICE_H__
