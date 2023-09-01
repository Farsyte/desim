#include "tau.h"

#include <assert.h>
#include <stdio.h>

void Tau_bist()
{
    // TAU and UNIT need to be more than 4 bytes.
    assert(sizeof(TAU) > 4);
    assert(sizeof(UNIT) > 4);
    printf("Tau_bist complete\n");
}
