#ifndef LIBX_MEMX_H
#define LIBX_MEMX_H

#include <libx/include/libx_def.h>

/* -------------------- GENERICS ------------------ */
/**
 * A generic 16 byte `Blob` structure for safely passing and casting types.
 * - `void* data`: This field stores an 8 byte pointer to some data. 
 * - `u32 stride`: This field stores stride metadata about the `Blob` structure.
 * - `u32 size`: This field stores size metadata about the `Blob` structure.
 *
 * #### NOTE: The `Blob::data` field can be safely converted to a `uptr` or `iptr` type.
 */
typedef struct Blob {
    void* data;
    u64 size;
} Blob;
/* -------------------- GENERICS ------------------ */


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

typedef struct Memx {
    u8 init;
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
} Memx;

LIBX_EXPOSE u8 libx_init_memx(void);
LIBX_EXPOSE void libx_cleanup_memx(void);

#endif  // LIBX_MEMX_H