/* types.h - Part of `PIC_Programmer` Arduino library
 * Created by KorespondentAda
 * Released into the public domain
 *
 * This header file define types used by library and some for external usage
 */
#pragma once

#include <stdlib.h>             // Standart types
#include "Pin.h"                // Pin type
#include "Icsp.h"               // In-Circuit Serial Programming

namespace pic {

using Word = uint16_t;          // Data word - 14 bit

// Types for specific PIC devices
class Pic;
using Pic16F873 = Pic;          // PIC16F873

}

