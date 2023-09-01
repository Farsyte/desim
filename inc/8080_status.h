
// Even with --std=gnu99, asking for -Wpedantic causes any use
// of these macros to generate
//     warning: binary constants are a C2X feature or GCC extension
//
// and all discussions I can find on how to eliminate that warning
// come down to "stop using that extension" because the answering
// idiot thinks that, just because they don't see value in the binary
// constant, nobody else should want to use it.
//
// sigh.
//
// To use this, I would have to bracket all USES of these macros
// with
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wpedantic"
// (use STATUS_whatever)
// #pragma GCC diagnostic pop

#ifdef	USE_BINARY_NUMERIC_LITERALS

// Position of each status bit in the status byte
#define STATUS_INTA  0b00000001 // Interrupt Acknowledge
#define STATUS_WO    0b00000010 // Write Output (active low)
#define STATUS_STACK 0b00000100 // Stack Memory Access
#define STATUS_HLTA  0b00001000 // Halt Acknowledge
#define STATUS_OUT   0b00010000 // Output Write
#define STATUS_M1    0b00100000 // Instruction Fetch (including Interrupt Service)
#define STATUS_INP   0b01000000 // Input Read
#define STATUS_MEMR  0b10000000 // Memory Read

// Aggregate Status Byte values for each machine cycle type
#define STATUS_RESET    0b00000010      // (0) can be used during RESET
#define STATUS_FETCH    0b10100010      // (1) MEMR M1
#define STATUS_MREAD    0b10000010      // (2) MEMR
#define STATUS_MWRITE   0b00000000      // (3)              /WO
#define STATUS_SREAD    0b10000110      // (4) MEMR STACK
#define STATUS_SWRITE   0b00000100      // (5)      STACK   /WO
#define STATUS_INPUTRD  0b01000010      // (6) INP
#define STATUS_OUTPUTWR 0b00010000      // (7) OUT          /WO
#define STATUS_INTACK   0b00100011      // (8)      M1        INTA
#define STATUS_HALTACK  0b10001010      // (9) MEMR    HLTA
#define STATUS_INTACKW  0b00101011      // (10)     M1 HLTA   INTA

#else

// This is the 8080. Use OCTAL.

// Position of each status bit in the status byte
#define STATUS_INTA   0001      // Interrupt Acknowledge
#define STATUS_WO     0002      // Write Output (active low)
#define STATUS_STACK  0004      // Stack Memory Access
#define STATUS_HLTA   0010      // Halt Acknowledge
#define STATUS_OUT    0020      // Output Write
#define STATUS_M1     0040      // Instruction Fetch (including Interrupt Service)
#define STATUS_INP    0100      // Input Read
#define STATUS_MEMR   0200      // Memory Read

// Aggregate Status Byte values for each machine cycle type
#define STATUS_RESET    0002    // (0) can be used during RESET
#define STATUS_FETCH    0242    // (1) MEMR M1
#define STATUS_MREAD    0202    // (2) MEMR
#define STATUS_MWRITE   0000    // (3)              /WO
#define STATUS_SREAD    0206    // (4) MEMR STACK
#define STATUS_SWRITE   0004    // (5)      STACK   /WO
#define STATUS_INPUTRD  0102    // (6) INP
#define STATUS_OUTPUTWR 0020    // (7) OUT          /WO
#define STATUS_INTACK   0043    // (8)      M1        INTA
#define STATUS_HALTACK  0212    // (9) MEMR    HLTA
#define STATUS_INTACKW  0053    // (10)     M1 HLTA   INTA
#endif
