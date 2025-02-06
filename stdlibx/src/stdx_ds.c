#include "../include/stdx_ds.h"

void* stdx_create_array(u32 stride, u32 capacity) {
    u32* head = (u32*)stdx_alloc(STDX_ARRAY_HEAD_SIZE + (stride * capacity), 16);
    if (!head) return NULL; // error: out of memory!

    head[STDX_ARRAY_SIZE_FIELD] = STDX_ARRAY_HEAD_SIZE + (stride * capacity);
    head[STDX_ARRAY_STRIDE_FIELD] = stride;
    head[STDX_ARRAY_COUNT_FIELD] = 0;
    head[STDX_ARRAY_CAPACITY_FIELD] = capacity;

    return (void*)((u8*)head + STDX_ARRAY_HEAD_SIZE);
}

Stdx_Array_Head stdx_arrhead(void* array) {
    Stdx_Array_Head arrhead = {0, 0, 0, 0};
    if (!array) return arrhead; // error: null ptr!

    u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
    
    arrhead.size = head[STDX_ARRAY_SIZE_FIELD];
    arrhead.stride = head[STDX_ARRAY_STRIDE_FIELD];
    arrhead.count = head[STDX_ARRAY_COUNT_FIELD];
    arrhead.capacity = head[STDX_ARRAY_CAPACITY_FIELD];

    return arrhead;
}

void stdx_destroy_array(void* array) {
    u32 head_size = (sizeof(u32) * 4);
    stdx_free((void*)((u8*)array - STDX_ARRAY_HEAD_SIZE));
}
