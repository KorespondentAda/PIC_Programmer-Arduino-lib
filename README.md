# PIC\_Programmer Arduino Library
Library for programming PIC MCUs through Arduino board.

## Installation
User can just clone this repository to `libraries` directory in your sketchbook path (by default in `C:\Users\<Username>\Documents\Arduino` on Windows, `~/Arduino` on Linux).
Then you can include this library in current sketch from menu by Sketch-\>Include Library-\>PIC\_Programmer.

## Usage

Class `PIC_Programmer` has interface methods that works with PIC MCU.

For programming a chip:
1. Create an `PIC_Programmer` object, pass working pin numbers to constructor;
2. Call `Init()` method of created object before start work;
3. After calling `ReadConfiguration()` you can see current configuration on serial monitor as readable data after calling `PrintConfiguration()`;
4. To read all data from chip to serial as binary call `ReadChip()`;
5. Writing hex dump to chip possible via serial after calling `WriteChip()`.

