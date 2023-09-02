#include "traced.h"

#include <stdio.h>
#include <stdlib.h>

#include "edge.h"
#include "stub.h"

void Traced_init(Traced t, Edge e, int active_low)
{
    t->valc = ' ';
    t->vals = (active_low) ? "#," : ",#";

    EDGE_RISE(e, Traced_rise, t);
    EDGE_FALL(e, Traced_fall, t);

    t->valc = t->vals[Edge_get(e)];
    t->u0 = UNIT;
    t->u = UNIT;
    t->boring_char = ',';
}

void Traced_active_boring(Traced t) {
    t->boring_char = '#';
}

static void append(Traced t, char valc)
{
    size_t              c = t->trace_cap;
    size_t              l = t->trace_len;
    char               *b = t->trace_buf;
    if (c <= l) {
        if (l < 8)
            c = 8;
        else
            c = l * 2;
        b = realloc(b, c);
        t->trace_buf = b;
        t->trace_cap = c;
    }
    b[l++] = valc;
    t->trace_len = l;
}

void Traced_update(Traced t)
{
    Tau                 u = t->u;
    while (u < UNIT) {
        append(t, t->valc);
        u++;
    }
    t->u = u;
}

static void Traced_set(Traced t, char newc)
{
    if (t->valc == newc)
        return;
    Traced_update(t);
    t->valc = newc;
}

void Traced_rise(Traced t)
{
    Traced_set(t, t->vals[1]);
}

void Traced_fall(Traced t)
{
    Traced_set(t, t->vals[0]);
}

void Traced_print(Traced t, Tau umin, Tau ulen)
{
    umin -= t->u0;

    Tau                 l = t->trace_len;
    if (umin >= l)
        return;
    if (ulen > l - umin)
        ulen = l - umin;

    const char         *s = t->trace_buf + umin;
    int                 boring = 1;
    char bc = t->boring_char;
    for (Tau i = 1; i < ulen; ++i)
        if (s[i] != bc)
            boring = 0;
    if (!boring)
        printf("    %s:\t%.*s\n", t->name, (int)ulen, s);
}
