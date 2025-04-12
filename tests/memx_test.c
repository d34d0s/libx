#include <libx/libx.h>

u8 bit_set(u8* byte, u8 bit) {
    return (!byte || !bit || bit > 8) ? LIBX_FALSE : (*byte |= (1 << bit));
}

u8 bit_rem(u8* byte, u8 bit) {
    return (!byte || !*byte || !bit || bit > 8) ? LIBX_FALSE : (*byte &= ~(1 << bit));
}

u8 bit_mask(u8 byte, u8 mask) {
    return (!byte || !mask) ? LIBX_FALSE : ((byte & mask) == mask);
}

void main() {
    libx_init(LIBX_MEMX);

    f32* mem = libx->memx.alloc((10 * sizeof(f32)), 16);
    if (!mem) printf("Failed to allocate memory!\n");
    
    mem[0] = 123321.0f;
    mem[1] = 420.0f;
    
    mem = libx->memx.realloc(mem, (20 * sizeof(f32)), 16);
    if (!mem) printf("Failed to reallocate memory!\n");

    LIBX_FORI(0, 2, 1) printf("value: %0.1f\n", mem[i]);

    libx->memx.dealloc(mem);
    
    Allocator lalloc; 
    libx->memx.create_allocator(ALLOC_ARENA, LIBX_MB, 8, &lalloc);
    printf("Arena Size: %d\n", lalloc.context.arena.max);

    u32* a = libx->memx.arena_alloc(sizeof(u32), 4, &lalloc);
    u32* b = libx->memx.arena_alloc(sizeof(f32), 8, &lalloc);
    u64* c = libx->memx.arena_alloc(sizeof(u64) * 2, 16, &lalloc);
    printf("a: %p\n", a);
    printf("b: %p\n", b);
    printf("c: %p\n", c);

    c[0] = 420;
    c[1] = 666;
    printf("c[0]: %d\n", c[0]);
    printf("c[1]: %d\n", c[1]);

    libx->memx.destroy_allocator(&lalloc);

    Blob blob = {.size = sizeof(f32) * 5};
    libx->memx.blob_alloc(&blob, 8);
    ((f32*)blob.data)[0] = 4.0; // store an iterator value within the blob
    LIBX_FORI(1, 5, 1) ((f32*)blob.data)[i] = 418.0 + i;
    LIBX_FORI(1, ((f32*)blob.data)[0]+1, 1) printf("blob.data[%d] = %0.1f\n", i, *(f32*)(((u8*)blob.data) + (sizeof(f32) * i)));
    libx->memx.blob_dealloc(&blob);

    if (libx_cleanup()) printf("Memx Test Ran!\n");
}
