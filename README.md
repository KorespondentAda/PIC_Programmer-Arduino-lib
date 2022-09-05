# PIC\_Programmer Arduino Library
Library for programming PIC MCUs through Arduino board.

## Installation
You can just clone this repository to `libraries` directory in your sketchbook path (by default in `C:\Users\<Username>\Documents\Arduino` on Windows, `~/Arduino` on Linux).
Then you can include this library to current sketch in Arduino IDE from menu by Sketch-\>Include Library-\>PIC\_Programmer, or just add `#include <Pic.h>` to your sketch.

## Usage

Class `Pic` has interface methods that works with PIC MCU.
That is universal base class for working with PICs. Classes specific for some devices are derived from `Pic`.

Library usage simple enought to work with like demonstrated in examples.

## Development

Currently supported only PIC16F873 through `Pic` class.
Supporting of all mid-range PIC devices planned.

