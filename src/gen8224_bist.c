#include "gen8224.h"

#include <stdio.h>

#include "clock.h"

static Gen8224      gen = { {"BIST8224", CLOCK} };

void Gen8224_bist()
{
    Gen8224_init(gen);

    printf("Gen8224_bist complete\n");
}
