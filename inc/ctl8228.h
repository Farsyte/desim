#pragma once

#include "edge.h"

typedef struct {
    const char         *name;

    // Change from 8228: we observe Data but do not provide bus driver
    // facilities. This seemed to me to be more of an analog thing
    // (supporting large fan-out of electronic signals).
    //
    // This means we do not even represent /BUSEN in the sim.
    //
    // Change from 8228: the 8228 emits /MEMR /IOR /INTA a brief time
    // after the /STSTB↓ edge, where the discrete logic does not emit
    // them until DBIN. This sim synchronizes their falling edge with
    // /STSTB↑ to avoid the need for a FIFO or (worse) a priority
    // queue smack in the middle of timing critical code in Clock.

    // Start with the edges we do not own, to make it easier to
    // hook them up in a static initializer.

    // Note that I am using Data, not D, to make it easier to
    // have the 8080 provide "D" as its D register.

    Byte               *Data;

    pEdge               STSTB_;
    pEdge               DBIN;
    pEdge               WR_;
    pEdge               HLDA;

    Edge                MEMR_;
    Edge                MEMW_;
    Edge                IOR_;
    Edge                IOW_;
    Edge                INTA_;
    // internal state: the status we latched

    Byte                status;

} Ctl8228[1];

extern unsigned     Ctl8228_debug;

extern void         Ctl8228_init(Ctl8228);
extern void         Ctl8228_linked(Ctl8228);
extern void         Ctl8228_bist();
