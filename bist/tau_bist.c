#include <assert.h>
#include <stdio.h>
#include "bist_macros.h"
#include "tau.h"

void Tau_bist()
{
    PRINT_TOP();
    // Tau values need to be more than 4 bytes.
    assert(sizeof(Tau) > 4);
    PRINT_END();
}
