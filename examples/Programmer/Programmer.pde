#include <Programmer.h>

pic::Programmer controller(2, 3, 4, 8);

void setup() {
    controller.Init();
    controller.EraseChip();
    controller.ReadConfiguration(false);
    controller.PrintConfiguration();
    controller.RewriteChip();
}

void loop() {
}
