#include <Programmer.h>

pic::Programmer controller(2, 3, 4, 8);

// Read configuration, write new config word, read it again to check
void setup() {
    controller.Init();
    controller.ReadConfiguration(false);
    controller.PrintConfiguration();

    auto newConf = 0b11111110110111;
    controller.SetConfigurationWord(newConf);
    Serial.print("Now configuration word must be ");
    Serial.println(newConf);
    controller.WriteConfiguration();
    Serial.println("Config written");

    controller.ReadConfiguration(false);
    controller.PrintConfiguration();
}

void loop() {
}
