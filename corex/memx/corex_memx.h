#ifndef __COREX_MEMX_H__
#define __COREX_MEMX_H__

#include <corex/corex_def.h>

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
    byte init;
    void (*dealloc)(void* ptr);
    void (*zero)(void* ptr, u64 size);
    void* (*alloc)(u64 size, u64 align);
    void* (*realloc)(void* ptr, u64 size, u64 align);

    byte (*blob_alloc)(Blob* blob, u64 align);
    byte (*blob_realloc)(Blob* blob, u64 size, u64 align);
    byte (*blob_dealloc)(Blob* blob);

    byte (*create_allocator)(Alloc_Type type, u64 max, u64 align, Allocator* allocator);
    byte (*destroy_allocator)(Allocator* allocator);

    void* (*arena_alloc)(u64 size, u64 align, Allocator* allocator);
    void (*arena_reset)(Allocator* allocator);
} Memx;

byte _corex_init_memx(void);
void _corex_cleanup_memx(void);

#endif  // COREX_MEMX_H