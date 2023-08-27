#pragma once

// convenience macro modules can use
// to assure that an un-owned field
// has been linked.

#define LINK_ASSERT(e) link_assert(e, #e, __FILE__, __LINE__, __PRETTY_FUNCTION__)
extern void link_assert(void* e, const char* n, const char* s, int l, const char* f);
