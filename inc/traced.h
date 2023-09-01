#pragma once

#include <stddef.h>

#include "edge.h"
#include "sim_types.h"
#include "tau.h"

typedef struct {
    Name                name;
    char               *trace_buf;
    size_t              trace_cap;
    size_t              trace_len;
    char                valc;
    char               *vals;

    Tau                 u0;
    Tau                 u;
}                  *pTraced, Traced[1];

extern void         Traced_init(Traced t, Edge e, int active_low);
extern void         Traced_update(Traced t);
extern void         Traced_rise(Traced t);
extern void         Traced_fall(Traced t);
extern void         Traced_print(Traced t, Tau umin, Tau ulen);
