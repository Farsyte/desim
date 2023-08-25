#pragma once

// Simulator representation of single-byte (8-bit) storage.
//
// If we are simulating a system with Bytes that are smaller
// than the bytes in the simulation host, the simulation will
// have to deal with extra bits.
//
// If we are simluating a system with Bytes that are larger
// than the bytes in the simulation host, then this definition
// of Byte will need to be modified.
//
// The need to modify Byte is expected to be vanishingly probable.
//
// It is more likely that we might want to redefine Byte as a class
// that allows for some "behind the scenes" operations.

#include <cstdint>
typedef uint8_t Byte;
