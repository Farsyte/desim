#include "tau.h"

#include <assert.h>

void tau_bist()
{
    // TAU and UNIT need to be more than 4 bytes.
    assert(sizeof(TAU) > 4);
    assert(sizeof(UNIT) > 4);
}
