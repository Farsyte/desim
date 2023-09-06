#include "util.h"
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

Cstr format(Cstr fmt, ...)
{
    va_list             ap;
    char               *result;
    int                 length;
    int                 actual;

    // int vsnprintf(char *str, size_t size, const char *format, va_list ap);
    //
    //    The function vsnprintf() does not write more than size
    //    bytes, including the terminating NUL. If the output was
    //    truncated due to this limit, then the return value is the
    //    number of characters (EXCLUDING the terminating null byte)
    //    which WOULD HAVE BEEN written to the final string if enough
    //    space had been available. Thus, a return value of size or more
    //    means that the output was truncated.

    // va_start(ap, fmt);
    // vfprintf(stderr, fmt, ap);
    // va_end(ap);

    va_start(ap, fmt);
    length = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);

    length += 1;        // add the terminating NUL
    result = malloc(length);

    va_start(ap, fmt);
    actual = vsnprintf(result, length, fmt, ap);
    va_end(ap);

    assert(actual <= length);
    return result;
}
