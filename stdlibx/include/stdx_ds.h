#pragma once

#include "stdx_mem.h"

#define STDX_ARRAY_HEAD_SIZE (sizeof(u32) * 4)

typedef struct Stdx_Array_Head {
    u32 size, stride;
    u32 count, capacity;
} Stdx_Array_Head;

typedef enum Stdx_Array_Head_Field {
    STDX_ARRAY_SIZE_FIELD = 0,
    STDX_ARRAY_STRIDE_FIELD,
    STDX_ARRAY_COUNT_FIELD,
    STDX_ARRAY_CAPACITY_FIELD,
    STDX_ARRAY_FIELDS,
} Stdx_Array_Head_Field;

STDX_API void* stdx_create_array(u32 stride, u32 capacity);
STDX_API Stdx_Array_Head stdx_arrhead(void* array);
STDX_API void stdx_destroy_array(void* array);
