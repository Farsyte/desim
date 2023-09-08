#pragma once
#include "edge.h"

#define SIO_RXRING_MASK	0377
#define SIO_TXRING_MASK	0377

typedef struct {
    const char         *name;
    pReg8               Data;

    Edge                RD;
    Edge                WR;

    char                rxring[SIO_RXRING_MASK + 1];
    Tau                 rxprod;
    Tau                 rxcons;
    Cstr                rxname;
    int                 rx;

    char                txring[SIO_TXRING_MASK + 1];
    Tau                 txprod;
    Tau                 txcons;
    Cstr                txname;
    int                 tx;

}                  *pSio8080, Sio8080[1];

extern void         Sio8080_init(Sio8080);
extern void         Sio8080_linked(Sio8080);
extern void         Sio8080_down(Sio8080);

extern void         Sio8080_bist();
