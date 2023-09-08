#pragma once
#include "edge.h"

// provide tty-style input and output
// - output a character
// - is there input available?
// - consume next input character

typedef struct {
    const char         *name;
    pByte               Data;
    Edge                WR;
    Edge                RD;
}                  *pTty8080, Tty8080[1];

extern void         Tty8080_init(Tty8080);
extern void         Tty8080_linked(Tty8080);
extern void         Tty8080_open(Tty8080 tty);
extern void         Tty8080_putc(Tty8080 tty, Byte ch);
extern void         Tty8080_getc(Tty8080 tty, Byte ch);
extern void         Tty8080_update(Tty8080 tty, Byte ch);
extern void         Tty8080_close(Tty8080 tty);

extern void         Tty8080_bist();
