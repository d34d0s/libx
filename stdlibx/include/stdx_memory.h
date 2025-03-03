#pragma once

#include "stdx_def.h"

#define IP2(x) ((x & (x - 1)) == 0)
#define ALIGN(p, a) (((p) + ((a) - 1)) & ~((a) - 1))

/* ---------------- LINEAR ALLOCATOR ---------------- */
typedef struct Linear_Allocator {
    void* data;
    u64 offset;
    u64 max;
} Linear_Allocator;
/* ---------------- LINEAR ALLOCATOR ---------------- */


/* ---------------- ARENA ALLOCATOR ---------------- */
typedef struct Arena_Allocator {
    struct Arena_Allocator* last;
    struct Arena_Allocator* next;
    void* data;
    u64 offset;
    u64 max;
} Arena_Allocator;
/* ---------------- ARENA ALLOCATOR ---------------- */


/* ---------------- STACK ALLOCATOR ---------------- */
/* ---------------- STACK ALLOCATOR ---------------- */


/* ---------------- RING ALLOCATOR ---------------- */
/* ---------------- RING ALLOCATOR ---------------- */


typedef struct _stdx_memory_api {
    void (*dealloc)(void* ptr);
    void* (*alloc)(u64 size, u64 align);
    void* (*realloc)(void* ptr, u64 size, u64 align);

    Linear_Allocator* (*create_linear_allocator)(u64 max, u64 align);
    void* (*linear_alloc)(Linear_Allocator* allocator, u64 size, u64 align);
    void (*linear_reset)(Linear_Allocator* allocator);
    void (*destroy_linear_allocator)(Linear_Allocator* allocator);

    Arena_Allocator* (*create_arena_allocator)(Arena_Allocator* allocator, u64 max, u64 align);
    void* (*arena_alloc)(Arena_Allocator* allocator, u64 size, u64 align);
    void (*arena_reset)(Arena_Allocator* allocator);
    void (*destroy_arena_allocator)(Arena_Allocator* allocator);
    void (*collapse_arena_allocator)(Arena_Allocator* allocator);
} _stdx_memory_api;
extern _stdx_memory_api* memory_api;

STDX_API u8 stdx_init_memory(void);
STDX_API void stdx_cleanup_memory(void);
