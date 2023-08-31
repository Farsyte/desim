#pragma once

#include <stddef.h>

#include "tau.h"
#include "sim_types.h"

typedef void Edge_fn (void *);

typedef struct Edge_fn_arg_s {
    Edge_fn            *fn;
    void               *arg;
} Edge_fn_arg;

typedef struct Edge_fn_args_s {
    size_t              count;
    size_t              capacity;
    Edge_fn_arg        *list;
} Edge_fn_args;

typedef struct Edge_s {
    Edge_fn_args        rise;
    Edge_fn_args        fall;
    Bit                 value;
} Edge;

extern void         Edge_init(Edge *e);

#define Edge_rise(e,fn,arg)	_Edge_rise(e, (Edge_fn *)fn, (void *)arg)
extern void         _Edge_rise(Edge *e, Edge_fn *fn, void *arg);

#define Edge_fall(e,fn,arg)	_Edge_fall(e, (Edge_fn *)fn, (void *)arg)
extern void         _Edge_fall(Edge *e, Edge_fn *fn, void *arg);

extern void         Edge_set(Edge *e, Bit value);
extern void         Edge_hi(Edge *e);
extern void         Edge_lo(Edge *e);

extern void         Edge_bist();
