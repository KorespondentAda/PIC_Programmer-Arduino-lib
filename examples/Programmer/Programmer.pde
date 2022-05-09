#include <Programmer.h>

pic::Programmer controller(2, 3, 4, 8);

// Read configuration, write new config word, read it again to check
void setup() {
    controller.Init();
    controller.ReadConfiguration();
    controller.PrintConfiguration();
    controller.ReadProgram(64);
}

void loop() {
}
