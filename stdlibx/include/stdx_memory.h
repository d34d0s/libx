#pragma once

#include "stdx_def.h"

#define IP2(x) ((x & (x - 1)) == 0)
#define ALIGN(p, a) (((p) + ((a) - 1)) & ~((a) - 1))

/* ---------------- ARENA ALLOCATOR ---------------- */
/* ---------------- ARENA ALLOCATOR ---------------- */


/* ---------------- LINKED ALLOCATOR ---------------- */
/* ---------------- LINKED ALLOCATOR ---------------- */


typedef struct _stdx_memory_api {
    void (*dealloc)(void* ptr);
    void* (*alloc)(u64 size, u64 align);
    void* (*realloc)(void* ptr, u64 size, u64 align);
} _stdx_memory_api;
extern _stdx_memory_api* memory_api;

STDX_API u8 stdx_init_memory(void);
STDX_API void stdx_cleanup_memory(void);
