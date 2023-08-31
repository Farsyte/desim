#pragma once

#include <stddef.h>

#include "tau.h"
#include "sim_types.h"

typedef void       *EdgeArg;
typedef void EdgeFn (EdgeArg);
typedef EdgeFn     *EdgeFp;

typedef struct {
    EdgeFp              fn;
    EdgeArg             arg;
} sEdgeFA;

typedef sEdgeFA    *pEdgeFA;

typedef struct {
    size_t              count;
    size_t              capacity;
    pEdgeFA             list;
} EdgeSL[1];

typedef struct {
    Name                name;
    Bit                 value;
    EdgeSL              rise;
    EdgeSL              fall;
} sEdge;

typedef sEdge Edge[1];
typedef sEdge      *pEdge;

extern unsigned     Edge_debug;

extern void         Edge_init(Edge e);

#define EDGE_RISE(e,fn,arg)	Edge_rise(e, (EdgeFp)fn, (EdgeArg)arg, #fn, #arg)
#define EDGE_FALL(e,fn,arg)	Edge_fall(e, (EdgeFp)fn, (EdgeArg)arg, #fn, #arg)

extern void         Edge_rise(Edge e, EdgeFp fn, EdgeArg arg,
                              Name fn_name, Name arg_name);

extern void         Edge_fall(Edge e, EdgeFp fn, EdgeArg arg,
                              Name fn_name, Name arg_name);

extern void         Edge_set(Edge e, Bit value);
extern void         Edge_hi(Edge e);
extern void         Edge_lo(Edge e);

extern void         Edge_bist();
