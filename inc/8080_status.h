// Position of each status bit in the status byte
#define STATUS_INTA  0b00000001; // Interrupt Acknowledge
#define STATUS_WO    0b00000010; // Write Output (active low)
#define STATUS_STACK 0b00000100; // Stack Memory Access
#define STATUS_HLTA  0b00001000; // Halt Acknowledge
#define STATUS_OUT   0b00010000; // Output Write
#define STATUS_M1    0b00100000; // Instruction Fetch (including Interrupt Service)
#define STATUS_INP   0b01000000; // Input Read
#define STATUS_MEMR  0b10000000; // Memory Read

// Aggregate Status Byte values for each machine cycle type
#define STATUS_RESET    0b00000010 // (0) can be used during RESET
#define STATUS_FETCH    0b10100010 // (1) MEMR M1
#define STATUS_MREAD    0b10000010 // (2) MEMR
#define STATUS_MWRITE   0b00000000 // (3)              /WO
#define STATUS_SREAD    0b10000110 // (4) MEMR STACK
#define STATUS_SWRITE   0b00000100 // (5)      STACK   /WO
#define STATUS_INPUTRD  0b01000010 // (6) INP
#define STATUS_OUTPUTWR 0b00010000 // (7) OUT          /WO
#define STATUS_INTACK   0b00100011 // (8)      M1        INTA
#define STATUS_HALTACK  0b10001010 // (9) MEMR    HLTA
#define STATUS_INTACKW  0b00101011 // (10)     M1 HLTA   INTA

// Bit declarations to be used inside a C Struct placing the
// symbols to match up with their positions in STATUS.
//
// Note that bits go from low to high on little-endian simulation
// hosts; to add support for big-endian simulation hosts, reverse
// the order of these lines.
#define STATUS_BIT_DECLS \
    unsigned sINTA  : 1; \
    unsigned sWO    : 1; \
    unsigned sSTACK : 1; \
    unsigned sHLTA  : 1; \
    unsigned sOUT   : 1; \
    unsigned sM1    : 1; \
    unsigned sINP   : 1; \
    unsigned sMEMR  : 1