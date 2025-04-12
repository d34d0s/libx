#include <corex/corex.h>

u8 bit_set(u8* byte, u8 bit) {
    return (!byte || !bit || bit > 8) ? COREX_FALSE : (*byte |= (1 << bit));
}

u8 bit_rem(u8* byte, u8 bit) {
    return (!byte || !*byte || !bit || bit > 8) ? COREX_FALSE : (*byte &= ~(1 << bit));
}

u8 bit_mask(u8 byte, u8 mask) {
    return (!byte || !mask) ? COREX_FALSE : ((byte & mask) == mask);
}

void main() {
    corex_init(COREX_MEMX);

    f32* mem = corex->memx.alloc((10 * sizeof(f32)), 16);
    if (!mem) printf("Failed to allocate memory!\n");
    
    mem[0] = 123321.0f;
    mem[1] = 420.0f;
    
    mem = corex->memx.realloc(mem, (20 * sizeof(f32)), 16);
    if (!mem) printf("Failed to reallocate memory!\n");

    COREX_FORI(0, 2, 1) printf("value: %0.1f\n", mem[i]);

    corex->memx.dealloc(mem);
    
    Allocator lalloc; 
    corex->memx.create_allocator(ALLOC_ARENA, COREX_MB, 8, &lalloc);
    printf("Arena Size: %d\n", lalloc.context.arena.max);

    u32* a = corex->memx.arena_alloc(sizeof(u32), 4, &lalloc);
    u32* b = corex->memx.arena_alloc(sizeof(f32), 8, &lalloc);
    u64* c = corex->memx.arena_alloc(sizeof(u64) * 2, 16, &lalloc);
    printf("a: %p\n", a);
    printf("b: %p\n", b);
    printf("c: %p\n", c);

    c[0] = 420;
    c[1] = 666;
    printf("c[0]: %d\n", c[0]);
    printf("c[1]: %d\n", c[1]);

    corex->memx.destroy_allocator(&lalloc);

    Blob blob = {.size = sizeof(f32) * 5};
    corex->memx.blob_alloc(&blob, 8);
    ((f32*)blob.data)[0] = 4.0; // store an iterator value within the blob
    COREX_FORI(1, 5, 1) ((f32*)blob.data)[i] = 418.0 + i;
    COREX_FORI(1, ((f32*)blob.data)[0]+1, 1) printf("blob.data[%d] = %0.1f\n", i, *(f32*)(((u8*)blob.data) + (sizeof(f32) * i)));
    corex->memx.blob_dealloc(&blob);

    if (corex_cleanup()) printf("Memx Test Ran!\n");
}
