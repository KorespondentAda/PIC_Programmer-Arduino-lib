#include <Programmer.h>

pic::Programmer controller(2, 3, 4, 8);

// Read configuration, erase current programm, check that it's erased, write new program and check that it written properly
// Example .hex-file available in examples/Programmer/prog2write.hex
void setup() {
    controller.Init();
    controller.ReadConfiguration();
    controller.PrintConfiguration();
    controller.EraseProgram();
    controller.ReadProgram(0x90);
    controller.WriteProgram();
    controller.ReadProgram(0x90);
}

void loop() {
}
