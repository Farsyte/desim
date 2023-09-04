#pragma once

#define	I8080_NOP	(0000)
#define	I8080_HLT	(0166)
#define	I8080_DI	(0363)
#define	I8080_EI	(0373)

#include "sim_types.h"

extern Cstr         disassemble(Byte op);
