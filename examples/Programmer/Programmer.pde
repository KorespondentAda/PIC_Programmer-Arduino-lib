#include <PIC_Programmer.h>

PIC_Programmer pic(2, 3, 4, 8);

void setup() {
    Device dev(2400);       // PIC16F873 for testing
    pic.Init(dev);
    //pic.ReadChip();
    //pic.RewriteChip();
}

void loop() {
}
