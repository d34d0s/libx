#pragma once

#include "stdx_def.h"

/* ---------------- ARRAY ---------------- */
typedef struct Array_Head {
    u32 size, stride;
    u32 count, capacity;
} Array_Head;

typedef enum Array_Head_Field {
    STDX_ARRAY_SIZE_FIELD = 0,
    STDX_ARRAY_STRIDE_FIELD,
    STDX_ARRAY_COUNT_FIELD,
    STDX_ARRAY_CAPACITY_FIELD,
    STDX_ARRAY_FIELDS,
} Array_Head_Field;
/* ---------------- ARRAY ---------------- */


/* ---------------- LINKED ARRAY ---------------- */
typedef struct Linked_Array {} Linked_Array;
/* ---------------- LINKED ARRAY ---------------- */


/* ---------------- HASHMAP ---------------- */
typedef struct Hashmap {
    void** values;
    cstr* keys;
    u32 count;
    u32 max;
} Hashmap;
/* ---------------- HASHMAP ---------------- */


/* ---------------- QUAD TREE ---------------- */
typedef struct Quad_Tree {} Quad_Tree;
/* ---------------- QUAD TREE ---------------- */

typedef struct _stdx_structs_api {
    void (*destroy_array)(void* array);

    /**
     * `push_array` is used to ensure a value is inserted at the next available slot in an array BASED ON THE CURRENT NUMBER OF ELEMENTS within the array.
     * Be careful using `push_array` after `n` calls to `put_array` as `push_array` will skip the first `n` slots in an array, and may over-write the inserts.
     */
    void (*push_array)(void* array, void* invalue);

    /**
     * `pop_array` is used to ensure a value is returned from the last occupied slot in an array BASED ON THE CURRENT NUMBER OF ELEMENTS within the array.
     * Calling `pop_array` DOES NOT shift/reorder elements in the array, instead the value at the popped index is set to 0, maintaining the integrity of the structure of data within the array.
     * Be careful using `pop_array` after `n` calls to `put_array` as `push_array` will start at index `n - 1` in an array, and fail to return inserted values.
     */
    void (*pop_array)(void* array, void* outvalue);
    
    void* (*create_array)(u32 stride, u32 capacity);
    void* (*resize_array)(void* array, u32 capacity);
    
    /**
     * `put_array` is used to manually write data to an array.
     * The data written with `put_array` is over-written with `n` calls to `push_array`, where `n` is equal to the `index` inserted to, ONLY IF the `index` inserted to is greater than the number of elements in the array.
     * Calling `push_array` after `n` calls to `put_array` ensures the first `n` slots of the array are never over-written by subsequent calls to `push_array`.
     */
    void (*put_array)(void* array, u32 index, void* invalue);
    
    /**
     * `pull_array` is used to clear and return a value from a specific index in an array
     * Calling `pull_array` DOES NOT shift/reorder elements in the array, instead the value at the removed index is set to 0, maintaining the integrity of the structure of data within the array.
     */
    void (*pull_array)(void* array, u32 index, void* outvalue);

    /**
     * `get_array_head` is used to return the metadata about an array that is stored in memory along with the array.
     */
    Array_Head (*get_array_head)(void* array);

    Hashmap* (*create_hashmap)(u32 max);
    u8 (*set_hashmap)(Hashmap* hashmap, cstr key, void* value);
    void* (*get_hashmap)(Hashmap* hashmap, cstr key);
    u8 (*rem_hashmap)(Hashmap* hashmap, cstr key);
    void (*destroy_hashmap)(Hashmap* hashmap);
} _stdx_structs_api;

/**
 * This API exists to implement various data-structures that are commonplace
 * in software-engineering.
 * 
 * The structs API follows the same vtable format any other stdx API.
 * 
 * STDX ARRAYS:
 * - stdx implements dynamically allocated arrays to be both static and resizeable.
 */
extern _stdx_structs_api* structs_api;

STDX_API u8 stdx_init_structs(void);
STDX_API void stdx_cleanup_structs(void);
