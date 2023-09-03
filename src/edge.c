#include "edge.h"

#include <assert.h>
#include <malloc.h>

#include "stub.h"
#include "tau.h"

unsigned            Edge_debug = 1;

#define NEW(p,n)	p = realloc(p, n * sizeof(*(p)))

static void Edge_list_init(EdgeSL list)
{
    list->count = 0;
    list->capacity = 0;
    list->list = 0;
}

void Edge_init(Edge e)
{
    if (Edge_debug > 0) {
        assert(e);
        assert(e->name && e->name[0]);
        if (Edge_debug > 1) {
            STUB(" Edge %p '%s'", (void *)e, e->name);
        }
    }
    e->value = 0;
    Edge_list_init(e->rise);
    Edge_list_init(e->fall);
}

static void Edge_add(Edge e, EdgeSL list, EdgeFn fn, void *arg,
                     Name rof, Name fn_name, Name arg_name)
{
    if (Edge_debug > 0) {
        assert(e);
        assert(e->name && e->name[0]);
        assert(rof && rof[0]);
        assert(fn_name && fn_name[0]);
        assert(arg_name && arg_name[0]);
        if (Edge_debug > 1) {
            STUB(" Edge %s %s calls %s(%s)", e->name, rof, fn_name,
                 arg_name);
        }
    }
    size_t              n = list->count;
    size_t              c = list->capacity;
    pEdgeFA             l = list->list;

    if (n >= c) {
        c = (n < 8) ? 8 : n * 2;
        NEW(l, c);
        list->list = l;
        list->capacity = c;
    }

    l[n].fn = fn;
    l[n].arg = arg;
    list->count = n + 1;
}

void Edge_rise(Edge e, EdgeFn fn, void *arg,
               Name fn_name, Name arg_name)
{
    Edge_add(e, e->rise, fn, arg, "rise", fn_name, arg_name);
}

void Edge_fall(Edge e, EdgeFn fn, void *arg,
               Name fn_name, Name arg_name)
{
    Edge_add(e, e->fall, fn, arg, "fall", fn_name, arg_name);
}

void Edge_run(EdgeSL sl)
{
    pEdgeFA             l = sl->list;
    pEdgeFA             e = l + sl->count;

    while (l < e) {
        EdgeFn              fn = l->fn;
        void               *arg = l->arg;

        fn(arg);
        l++;
    }
}

Bit Edge_get(Edge e)
{
    return e->value;
}

void Edge_set(Edge e, Bit value)
{
    if (value)
        Edge_hi(e);
    else
        Edge_lo(e);
}

void Edge_hi(Edge e)
{
    if (e->value)
        return;
    e->value = 1;
    Edge_run(e->rise);
}

void Edge_lo(Edge e)
{
    if (!e->value)
        return;
    e->value = 0;
    Edge_run(e->fall);
}
