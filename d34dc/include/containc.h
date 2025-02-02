#pragma once

#include "memc.h"

#define D34D_ARRAY_HEADER_SIZE (sizeof(u32) * 4)

typedef enum D34d_Array_Field {
    D34D_SIZE_FIELD = 0,
    D34D_STRIDE_FIELD,
    D34D_COUNT_FIELD,
    D34D_CAPACITY_FIELD,
    D34D_ARRAY_FIELDS,
} D34d_Array_Field;

PUBLIC_FUNC void* d34d_create_array(u32 stride, u32 capacity);
PUBLIC_FUNC void d34d_destroy_array(void* array);
