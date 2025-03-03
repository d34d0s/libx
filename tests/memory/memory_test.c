#include "../../stdlibx/include/stdx_memory.h"

void main() {
    stdx_init_memory();

    f32* mem = memory_api->alloc((10 * sizeof(f32)), 16);
    if (!mem) printf("Failed to allocate memory!\n");
    
    mem[0] = 123321.0f;
    mem[1] = 420.0f;
    
    mem = memory_api->realloc(mem, (20 * sizeof(f32)), 16);
    if (!mem) printf("Failed to reallocate memory!\n");

    STDX_FORI(0, 2, 1) printf("value: %0.1f\n", mem[i]);

    memory_api->dealloc(mem);
    
    
    Linear_Allocator* lalloc = memory_api->create_linear_allocator(1024, 16);

    u32* a = memory_api->linear_alloc(lalloc, sizeof(u32), 4);
    u32* b = memory_api->linear_alloc(lalloc, sizeof(f32), 8);
    u64* c = memory_api->linear_alloc(lalloc, sizeof(u64) * 2, 16);
    printf("a: %p\n", a);
    printf("b: %p\n", b);
    printf("c: %p\n", c);

    c[0] = 420;
    c[1] = 666;
    printf("c[0]: %d\n", c[0]);
    printf("c[1]: %d\n", c[1]);

    memory_api->destroy_linear_allocator(lalloc);

    
    Arena_Allocator* aalloc = memory_api->create_arena_allocator(NULL, sizeof(f32), 16);

    *(f32*)aalloc->data = 666.0;    // manually set root data
    printf("aalloc data: %0.1f\n", *(f32*)aalloc->data);

    // return an address from the arena buffer to set memory
    // however this wont fit in aalloc, so we get a new arena's buffer
    f32* values = memory_api->arena_alloc(aalloc, sizeof(f32) * 2, 16);
    values[0] = 456.123;
    values[1] = 333.444;
    printf("values[0]: %0.3f\n", values[0]);
    printf("values[1]: %0.3f\n", values[1]);
    
    printf("new arena offset: %d | should be %d\n", aalloc->next->offset, sizeof(f32)*2);
    printf("new arena max: %d | should be %d\n", aalloc->next->max, aalloc->next->last->max + sizeof(f32)*2);
    
    f32* values2 = memory_api->arena_alloc(aalloc->next, sizeof(f32) * 4, 16);

    memory_api->destroy_arena_allocator(aalloc); // collapse the entire arena

    stdx_cleanup_memory();
    printf("OK!\n");
}
