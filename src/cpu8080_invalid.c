#include "cpu8080_invalid.h"

#include <stdio.h>

#include "8080_instructions.h"

static void s_invalid(Cpu8080 cpu, Cpu8080_phase ph)
{
    (void)cpu;
    (void)ph;
}

static void s_invalid_loud(Cpu8080 cpu, Cpu8080_phase ph)
{
    printf("%s no instruction decode:\n", cpu->name);
    printf("  Addr: 0%o6o\n", *cpu->Addr);
    printf("  Data: 0%o6o\n", *cpu->Data);
    printf("  Inst: 0%o6o\n", *cpu->IR);

    cpu->state_next = s_invalid;
    cpu->state = cpu->state_next;
    cpu->state(cpu, ph);
}

void Cpu8080_init_invalid(Cpu8080 cpu)
{
    for (unsigned inst = 0; inst <= 0377; ++inst)
        cpu->M1T4[inst] = s_invalid_loud;

}
