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
    
    Linear_Allocator* lalloc = libx->memx.create_linear_allocator(1024, 16);

    u32* a = libx->memx.linear_alloc(lalloc, sizeof(u32), 4);
    u32* b = libx->memx.linear_alloc(lalloc, sizeof(f32), 8);
    u64* c = libx->memx.linear_alloc(lalloc, sizeof(u64) * 2, 16);
    printf("a: %p\n", a);
    printf("b: %p\n", b);
    printf("c: %p\n", c);

    c[0] = 420;
    c[1] = 666;
    printf("c[0]: %d\n", c[0]);
    printf("c[1]: %d\n", c[1]);

    libx->memx.destroy_linear_allocator(lalloc);

    
    Arena_Allocator* aalloc = libx->memx.create_arena_allocator(NULL, sizeof(f32), 16);

    *(f32*)aalloc->data = 666.0;    // manually set root data
    printf("aalloc data: %0.1f\n", *(f32*)aalloc->data);

    // return an address from the arena buffer to set memory
    // however this wont fit in aalloc, so we get a new arena's buffer
    f32* values = libx->memx.arena_alloc(aalloc, sizeof(f32) * 2, 16);
    values[0] = 456.123;
    values[1] = 333.444;
    printf("values[0]: %0.3f\n", values[0]);
    printf("values[1]: %0.3f\n", values[1]);
    
    printf("new arena offset: %d | should be %d\n", aalloc->next->offset, sizeof(f32)*2);
    printf("new arena max: %d | should be %d\n", aalloc->next->max, aalloc->next->last->max + sizeof(f32)*2);
    
    f32* values2 = libx->memx.arena_alloc(aalloc->next, sizeof(f32) * 4, 16);

    libx->memx.destroy_arena_allocator(aalloc); // collapse the entire arena    

    Blob blob = {.size = sizeof(f32) * 5};
    libx->memx.blob_alloc(&blob, 8);
    ((f32*)blob.data)[0] = 4.0; // store an iterator value within the blob
    LIBX_FORI(1, 5, 1) ((f32*)blob.data)[i] = 418.0 + i;
    LIBX_FORI(1, ((f32*)blob.data)[0]+1, 1) printf("blob.data[%d] = %0.1f\n", i, *(f32*)(((u8*)blob.data) + (sizeof(f32) * i)));
    libx->memx.blob_dealloc(&blob);

    if (libx_cleanup()) printf("Memx Test Ran!\n");
}
