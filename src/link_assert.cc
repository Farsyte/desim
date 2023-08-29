#include "link_assert.hh"

#include <cstdio>
#include <cstdlib>

void link_assert(void* e, const char* n, const char* s, int l, const char* f)
{
    if (e)
        return;
    fprintf(stderr, "%s:%d: '%s' not linked in %s\n", s, l, n, f);
    abort();
}
