#include "edge.h"

#include <malloc.h>
#include <assert.h>

#include "tau.h"

#define NEW(p,n)	p = realloc(p, n * sizeof(*(p)))

static void edge_list_init(edge_fn_args *list)
{
    list->count = 0;
    list->capacity = 0;
    list->list = 0;
}

void edge_init(edge *e)
{
    edge_list_init(&e->rise);
    edge_list_init(&e->fall);
    e->value = 0;
}

static void edge_add(edge_fn_args *list, edge_fn *fn, void *arg)
{
    size_t              n = list->count;
    size_t              c = list->capacity;
    edge_fn_arg        *l = list->list;

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

void _edge_rise(edge *e, edge_fn *fn, void *arg)
{
    edge_add(&e->rise, fn, arg);
}

void _edge_fall(edge *e, edge_fn *fn, void *arg)
{
    edge_add(&e->fall, fn, arg);
}

void edge_run(edge_fn_args *list)
{
    edge_fn_arg        *l = list->list;
    edge_fn_arg        *e = l + list->count;

    while (l < e) {
        l->fn(l->arg);
        l++;
    }
}

void edge_set(edge *e, bit value)
{
    bit                 old = e->value;

    if (value == old)
        return;

    e->value = value;

    if (value > old)
        edge_run(&e->rise);
    if (value < old)
        edge_run(&e->fall);
}
