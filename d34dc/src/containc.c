#include "../include/containc.h"

void* d34d_create_array(u32 stride, u32 capacity) {
    u32* header = (u32*)d34d_alloc(D34D_ARRAY_HEADER_SIZE + (stride * capacity), 16);
    if (!header) return NULL; // error: out of memory!

    header[0] = (stride * capacity);    // size
    header[1] = stride;
    header[2] = 0;                      // count
    header[3] = capacity;

    return (void*)((u8*)header + D34D_ARRAY_HEADER_SIZE);
}

void d34d_destroy_array(void* array) {
    u32 header_size = (sizeof(u32) * 4);
    d34d_free((void*)((u8*)array - D34D_ARRAY_HEADER_SIZE));
}
