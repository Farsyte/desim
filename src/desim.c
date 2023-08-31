#include <stdio.h>
#include <time.h>

#include "tau.h"
#include "edge.h"
#include "clock.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    Tau_bist();
    Edge_bist();
    Clock_bist();

    printf("PASS: the usual BIST are complete.\n");
}
