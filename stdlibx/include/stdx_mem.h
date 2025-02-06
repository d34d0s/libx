#include "stdx_def.h"

#define IP2(x) ((x & (x - 1)) == 0)
#define ALIGN(p, a) (((p) + ((a) - 1)) & ~((a) - 1))

STDX_API void stdx_free(void *ptr);
STDX_API void* stdx_alloc(u64 size, u64 align);
STDX_API void* stdx_realloc(void* ptr, u64 size, u64 align);
