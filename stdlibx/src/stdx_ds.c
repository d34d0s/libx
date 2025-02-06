#include "../include/stdx_ds.h"

STDX_DS_API* data_structs = NULL;

void* _create_array_impl(u32 stride, u32 capacity) {
    u32* head = (u32*)stdx_alloc(STDX_ARRAY_HEAD_SIZE + (stride * capacity), 16);
    if (!head) return NULL; // error: out of memory!

    head[STDX_ARRAY_SIZE_FIELD] = STDX_ARRAY_HEAD_SIZE + (stride * capacity);
    head[STDX_ARRAY_STRIDE_FIELD] = stride;
    head[STDX_ARRAY_COUNT_FIELD] = 0;
    head[STDX_ARRAY_CAPACITY_FIELD] = capacity;

    return (void*)((u8*)head + STDX_ARRAY_HEAD_SIZE);
}

void _insert_array_impl(void* array, u32 index, void* invalue) {
	if (!array || !invalue || index < 0) return;
	
	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
	if (index >= head[STDX_ARRAY_CAPACITY_FIELD] || (head[STDX_ARRAY_COUNT_FIELD] + 1) > head[STDX_ARRAY_CAPACITY_FIELD]) return;

	u32 offset = (head[STDX_ARRAY_STRIDE_FIELD] * index);

	void* slot = ((u8*)array + offset);
    memcpy(slot, invalue, head[STDX_ARRAY_STRIDE_FIELD]);

	head[STDX_ARRAY_COUNT_FIELD]++;
}

void _remove_array_impl(void* array, u32 index, void* outvalue) {
    if (!array || outvalue == NULL) return;
	
	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
	if (index >= head[STDX_ARRAY_CAPACITY_FIELD] || (head[STDX_ARRAY_COUNT_FIELD] - 1) < 0) return;

	u32 offset = (head[STDX_ARRAY_STRIDE_FIELD] * index);

	void* slot = ((u8*)array + offset);
    memcpy(outvalue, slot, head[STDX_ARRAY_STRIDE_FIELD]);

    memset(slot, 0, head[STDX_ARRAY_STRIDE_FIELD]);
    head[STDX_ARRAY_COUNT_FIELD]--;
}

void _push_array_impl(void* array, void* invalue) {
    if (!array || !invalue) return;
	
	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
    u32 index = head[STDX_ARRAY_COUNT_FIELD];

    data_structs->insert_array(array, index, invalue);
}

void _pop_array_impl(void* array, void* outvalue) {
    if (!array || outvalue == NULL) return;
	
	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
    if (!head[STDX_ARRAY_COUNT_FIELD]) return;

    u32 index = (head[STDX_ARRAY_COUNT_FIELD] - 1);

    data_structs->remove_array(array, index, outvalue);
}

void* _resize_array_impl(void* array, u32 capacity) {
    if (!array || !capacity) array;

	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
    
    u32 count = head[STDX_ARRAY_COUNT_FIELD];
    u32 stride = head[STDX_ARRAY_STRIDE_FIELD];
    u32 newsize = STDX_ARRAY_HEAD_SIZE + (stride * capacity);

    u32* newhead = (u32*)stdx_realloc(head, newsize, 16);
    if (!newhead) return array;

    newhead[STDX_ARRAY_SIZE_FIELD] = STDX_ARRAY_HEAD_SIZE + (stride * capacity);
    newhead[STDX_ARRAY_STRIDE_FIELD] = stride;
    newhead[STDX_ARRAY_COUNT_FIELD] = count;
    newhead[STDX_ARRAY_CAPACITY_FIELD] = capacity;

    return (void*)((u8*)newhead + STDX_ARRAY_HEAD_SIZE);
}

void _destroy_array_impl(void* array) {
    stdx_dealloc((void*)((u8*)array - STDX_ARRAY_HEAD_SIZE));
}

Stdx_Array_Head _get_array_head_impl(void* array) {
    Stdx_Array_Head arrhead = {0, 0, 0, 0};
    if (!array) return arrhead; // error: null ptr!

    u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
    
    arrhead.size = head[STDX_ARRAY_SIZE_FIELD];
    arrhead.stride = head[STDX_ARRAY_STRIDE_FIELD];
    arrhead.count = head[STDX_ARRAY_COUNT_FIELD];
    arrhead.capacity = head[STDX_ARRAY_CAPACITY_FIELD];

    return arrhead;
}

void _set_array_head_impl(void* array, u32 field, u32 value) {
    if (!array || field >= STDX_ARRAY_FIELDS) return;
    u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
    head[field] = value;
}

u8 stdx_init_data_structs(void) {
    data_structs = (STDX_DS_API*)stdx_alloc(sizeof(STDX_DS_API), 16);
    if (!data_structs) return 0;

    data_structs->destroy_array = _destroy_array_impl;
    data_structs->push_array = _push_array_impl;
    data_structs->pop_array = _pop_array_impl;
    data_structs->create_array = _create_array_impl;
    data_structs->resize_array = _resize_array_impl;
    data_structs->insert_array = _insert_array_impl;
    data_structs->remove_array = _remove_array_impl;
    
    data_structs->get_array_head = _get_array_head_impl;

    return 1;
}

void stdx_cleanup_data_structs(void) {
    stdx_dealloc(data_structs);
}

