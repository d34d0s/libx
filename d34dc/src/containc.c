#include "../include/containc.h"

void* d34d_create_array(u32 stride, u32 capacity) {
    u32 header_size = (sizeof(u32) * 4);
    u32* header = (u32*)d34d_alloc(header_size + (stride * capacity), 16);
    if (!header) return NULL; // error: out of memory!

    header[0] = (stride * capacity);    // size
    header[1] = stride;
    header[2] = 0;                      // count
    header[3] = capacity;

    return (void*)((u8*)header + header_size);
}

void d34d_destroy_array(void* array) {
    u32 header_size = (sizeof(u32) * 4);
    d34d_free((void*)((u8*)array - header_size));
}
