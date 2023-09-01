#pragma once

#include <stdint.h>

// provide Bit which is core to all simulations.
typedef uint8_t Bit;

// Code shared by all simuations
// makes use of pointers that are set to
// literal strings, or even just pointers to
// strings that should not change.
// I prefer "Cstr" over "const char *" for these.
typedef const char *Cstr;

// Code shared by all simuations
// attaches names to many things.
// It is useful to declare them as Name.
typedef Cstr Name;

// Definitions of Byte and Word may vary depending
// on what is being simulated.
typedef uint8_t Byte;
typedef uint16_t Word;
