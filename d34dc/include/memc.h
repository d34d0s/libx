#include "defines.h"

#define IP2(x) ((x & (x - 1)) == 0)
#define ALIGN(p, a) (((p) + ((a) - 1)) & ~((a) - 1))

PUBLIC_FUNC void d34d_free(void *ptr);
PUBLIC_FUNC void * d34d_alloc(u64 size, u64 align);
PUBLIC_FUNC void * d34d_realloc(void *ptr, u64 size, u64 align);
