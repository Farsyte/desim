#pragma once

// Simulator representation of single-bit storage.

// The need to modify Bit is expected to be vanishingly probable.
//
// It is more likely that we might want to redefine Bit as a class
// that allows for some "behind the scenes" operations.

typedef bool Bit;
