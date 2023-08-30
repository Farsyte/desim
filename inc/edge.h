#pragma once

#include <stddef.h>

#include "tau.h"
#include "sim_types.h"

typedef void edge_fn (void *);

typedef struct edge_fn_arg_s {
    edge_fn            *fn;
    void               *arg;
} edge_fn_arg;

typedef struct edge_fn_args_s {
    size_t              count;
    size_t              capacity;
    edge_fn_arg        *list;
} edge_fn_args;

typedef struct edge_s {
    edge_fn_args        rise;
    edge_fn_args        fall;
    bit                 value;
} edge;

extern void         edge_init(edge *e);

#define edge_rise(e,fn,arg)	_edge_rise(e, (edge_fn *)fn, (void *)arg)
extern void         _edge_rise(edge *e, edge_fn *fn, void *arg);

#define edge_fall(e,fn,arg)	_edge_fall(e, (edge_fn *)fn, (void *)arg)
extern void         _edge_fall(edge *e, edge_fn *fn, void *arg);

extern void         edge_set(edge *e, bit value);

extern void         edge_bist();
