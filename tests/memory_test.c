#include "../stdlibx/include/stdx_memory.h"

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

    stdx_cleanup_memory();
    printf("OK!\n");
}
