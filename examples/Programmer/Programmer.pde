/* Programmer.pde - Arduino example sketch
 * Created by KorespondentAda
 * Released into the public domain
 *
 * This sketch demonstrate various library usages
 */
#include <Pic.h>

pic::Pic16F873 device(2, 3, 4, 8);

int lastOp = 0;
pic::Word program[16] = {
	0x00DE, 0x01AD, 0x0200, 0x03BE, 0x04AF, 0x0500, 0x0600, 0x0700,
	0x0800, 0x0900, 0x0A00, 0x0B00, 0x0C00, 0x0D00, 0x0E00, 0x0F00
};

void setup() {
	Serial.begin(9600);
	while (!Serial);
	delay(500);
	Serial.println("\n\nProgram started");
}

void loop() {
	Serial.println("Enter command.");
	while (!Serial.available());
	int operation = Serial.read();
	switch (operation) {
		case '0':
			Help();
			break;
		case '1':
			ReadConfiguration();
			break;
		case '2':
			ClearConfiguration();
			break;
		case '3':
			Erase(1);
			break;
		case '4':
			Erase(2);
			break;
		case '5':
			Erase(0);
			break;
		case '6':
			ReadProgramPart(16);
			break;
		case '7':
			WriteProgram(program, 16);
			break;
		case '\n':
			break;
		default:
			Serial.println("Not a device command");
	}
}

void Help() {
	Serial.println("Available commands:");
	Serial.println("\t0: Show this help");
	Serial.println("\t1: Read Configuration");
	Serial.println("\t2: Clear Configuration");
	Serial.println("\t3: Erase Program");
	Serial.println("\t4: Erase Data");
	Serial.println("\t5: Erase Full");
	Serial.println("\t6: Read 16 program bytes");
	Serial.println("\t7: Write 16 program bytes");
}

void Erase(int n) {
	device.begin();
	Serial.print("Begin erasing: ");
	Serial.println(n == 1 ? "program memory"
	             : n == 2 ? "data memory"
	             : n == 0 ? "full (protected)"
	             : "Unimplemented eraser");
	device.erase(n);
	device.end();
}

void ReadProgramPart(int n) {
	device.begin();
	Serial.print("Reading ");
	Serial.print(n);
	Serial.println(" program words:");
	for (int i = 0; i < n; i++) {
		Serial.print("	");
		Serial.print(i, 16);
		Serial.print(": ");
		Serial.println(device.read(), 16);
		device.step();
	}
	Serial.println("Program reading done");
	Serial.println();
	device.end();
}

void WriteProgram(pic::Word *p, int wordCount) {
	Serial.print("Begin writing ");
	Serial.print(wordCount);
	Serial.println(" words:");
	device.begin();

	for (int i = 0; i < wordCount; i++) {
		Serial.print("	");
		Serial.print(i, 16);
		Serial.print(": ");
		Serial.println(p[i]);
		device.write(p[i]);
		device.step();
	}

	device.end();
	Serial.println("Program written");
}

void ReadConfiguration() {
	Serial.println("Begin configuration reading");
	device.begin();
	Serial.println("Start seeking...");
	device.seek(0x2000);
	Serial.print("ID1: ");
	Serial.println(device.read());
	device.step();
	Serial.print("ID2: ");
	Serial.println(device.read());
	device.step();
	Serial.print("ID3: ");
	Serial.println(device.read());
	device.step();
	Serial.print("ID4: ");
	Serial.println(device.read());
	device.step();
	Serial.print("Reserved: ");
	Serial.println(device.read());
	device.step();
	Serial.print("Reserved: ");
	Serial.println(device.read());
	device.step();
	Serial.print("DevID: ");
	Serial.println(device.read());
	device.step();
	Serial.print("ConfigWord: ");
	Serial.println(device.read());
	device.end();
	Serial.println("Configuration readed");
	Serial.println();
}

void WriteConfigurationWord(int w) {
	Serial.println("Writing new configuration word");
	device.begin();
	device.seek(0x2007);
	Serial.print("Current confWord: ");
	Serial.println(device.read());
	Serial.print("Writing new word: ");
	Serial.println(w);
	device.rewrite(w);
	device.end();
}

void ClearConfiguration() {
	Serial.println("Clearing configuration word (setting to 0x3FFF)");
	WriteConfigurationWord(0x3FFF);
}

