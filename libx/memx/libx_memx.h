#ifndef __LIBX_MEMX_H__
#define __LIBX_MEMX_H__

#include <libx/libx_def.h>

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

typedef enum Alloc_Type {
    ALLOC_DEFAULT,
    ALLOC_ARENA,
    ALLOC_TYPES
} Alloc_Type;

typedef struct Allocator {
    Alloc_Type type;
    union context {
        struct arena {
            void* buffer;
            u64 offset;
            u64 commit;
            u64 max;
        } arena;
    } context;
} Allocator;

typedef struct Memx {
    u8 init;
    void (*dealloc)(void* ptr);
    void (*zero)(void* ptr, u64 size);
    void* (*alloc)(u64 size, u64 align);
    void* (*realloc)(void* ptr, u64 size, u64 align);

    u8 (*blob_alloc)(Blob* blob, u64 align);
    u8 (*blob_realloc)(Blob* blob, u64 size, u64 align);
    u8 (*blob_dealloc)(Blob* blob);

    u8 (*create_allocator)(Alloc_Type type, u64 max, u64 align, Allocator* allocator);
    u8 (*destroy_allocator)(Allocator* allocator);

    void* (*arena_alloc)(u64 size, u64 align, Allocator* allocator);
    void (*arena_reset)(Allocator* allocator);
} Memx;

u8 _libx_init_memx(void);
void _libx_cleanup_memx(void);

#endif  // LIBX_MEMX_H