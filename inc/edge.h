#pragma once

#include <stddef.h>

#include "tau.h"
#include "sim_types.h"

//typedef void       *EdgeArg;
//typedef void EdgeFn (EdgeArg);

typedef void        (*EdgeFn)(void *);

typedef struct {
    EdgeFn              fn;
    void               *arg;
}                  *pEdgeFA, EdgeFA[1];

typedef struct {
    size_t              count;
    size_t              capacity;
    pEdgeFA             list;
}                  *pEdgeSL, EdgeSL[1];

typedef struct sEdge {
    Name                name;
    Bit                 value;
    EdgeSL              rise;
    EdgeSL              fall;
}                  *pEdge, Edge[1];

extern unsigned     Edge_debug;

extern void         Edge_init(Edge e);

#define EDGE_RISE(e,fn,arg)	Edge_rise(e, (EdgeFn)fn, (void *)arg, #fn, #arg)
#define EDGE_FALL(e,fn,arg)	Edge_fall(e, (EdgeFn)fn, (void *)arg, #fn, #arg)

extern void         Edge_rise(Edge e, EdgeFn fn, void *arg,
                              Name fn_name, Name arg_name);

extern void         Edge_fall(Edge e, EdgeFn fn, void *arg,
                              Name fn_name, Name arg_name);

extern void         Edge_set(Edge e, Bit value);
extern void         Edge_hi(Edge e);
extern void         Edge_lo(Edge e);

extern void         Edge_bist();
