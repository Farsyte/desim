#include "gen8224.h"

#include <assert.h>

#include "stub.h"

unsigned            Gen8224_debug = 1;


void Gen8224_init(Gen8224 gen)
{
    if (Gen8224_debug > 0) {
        assert(gen);
        assert(gen->name);
        assert(gen->name[0]);
        assert(gen->OSC);
        assert(gen->OSC->name);
        assert(gen->OSC->name[0]);
        if (Gen8224_debug > 1) {
            STUB(" Gen_8224 %p '%s'", (void *)gen, gen->name);
            STUB(" OSC connected to '%s'", gen->OSC->name);
        }
    }
}
