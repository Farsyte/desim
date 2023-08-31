#include "edge.h"

#include <malloc.h>
#include <assert.h>

#include "tau.h"

#define NEW(p,n)	p = realloc(p, n * sizeof(*(p)))

static void Edge_list_init(Edge_fn_args *list)
{
    list->count = 0;
    list->capacity = 0;
    list->list = 0;
}

void Edge_init(Edge *e)
{
    Edge_list_init(&e->rise);
    Edge_list_init(&e->fall);
    e->value = 0;
}

static void Edge_add(Edge_fn_args *list, Edge_fn *fn, void *arg)
{
    size_t              n = list->count;
    size_t              c = list->capacity;
    Edge_fn_arg        *l = list->list;

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

void _Edge_rise(Edge *e, Edge_fn *fn, void *arg)
{
    Edge_add(&e->rise, fn, arg);
}

void _Edge_fall(Edge *e, Edge_fn *fn, void *arg)
{
    Edge_add(&e->fall, fn, arg);
}

void Edge_run(Edge_fn_args *list)
{
    Edge_fn_arg        *l = list->list;
    Edge_fn_arg        *e = l + list->count;

    while (l < e) {
        l->fn(l->arg);
        l++;
    }
}

void Edge_set(Edge *e, Bit value)
{
    if (value)
        Edge_hi(e);
    else
        Edge_lo(e);
}

void Edge_hi(Edge *e)
{
    if (e->value)
        return;
    e->value = 1;
    Edge_run(&e->rise);
}
void Edge_lo(Edge *e)
{
    if (!e->value)
        return;
    e->value = 0;
    Edge_run(&e->fall);
}
