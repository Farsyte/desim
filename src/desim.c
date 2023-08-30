#include <stdio.h>
#include <time.h>

#include "edge.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    tau_bist();
    edge_bist();

    printf("PASS: the usual BIST are complete.\n");
}
