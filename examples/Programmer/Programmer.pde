#include <PIC_Programmer.h>

PIC_Programmer pic(2, 3, 4, 8);

void setup() {
    pic.Init();
    pic.ReadConfiguration(false);
    pic.PrintConfiguration();
    //pic.ReadChip();
    //pic.RewriteChip();
}

void loop() {
}
