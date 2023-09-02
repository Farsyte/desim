#include "tau.h"

#include <assert.h>
#include <stdio.h>

#include "bist_macros.h"

void Tau_bist()
{
    PRINT_TOP();
    // TAU and UNIT need to be more than 4 bytes.
    assert(sizeof(TAU) > 4);
    assert(sizeof(UNIT) > 4);
    PRINT_END();
}
