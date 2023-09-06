#include "8080_instructions.h"
#include "sim_types.h"

#define MATCH(op) case I8080_##op: return #op
Cstr disassemble(Byte op)
{
    switch (op) {
      default:
          return "";
          MATCH(NOP);
          MATCH(HLT);
          MATCH(DI);
          MATCH(EI);
    }
}
