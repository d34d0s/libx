#include "../include/stdx_structs.h"

#include "stdx_memory.h"

_stdx_structs_api* structs_api = NULL;

#define STDX_ARRAY_HEAD_SIZE (sizeof(u32) * 4)

void* _create_array_impl(u32 stride, u32 capacity) {
    u32* head = (u32*)memory_api->alloc(STDX_ARRAY_HEAD_SIZE + (stride * capacity), 16);
    if (!head) return NULL; // error: out of memory!

    head[STDX_ARRAY_SIZE_FIELD] = STDX_ARRAY_HEAD_SIZE + (stride * capacity);
    head[STDX_ARRAY_STRIDE_FIELD] = stride;
    head[STDX_ARRAY_COUNT_FIELD] = 0;
    head[STDX_ARRAY_CAPACITY_FIELD] = capacity;

    return (void*)((u8*)head + STDX_ARRAY_HEAD_SIZE);
}

void _writein_array_impl(void* array, u32 index, void* invalue) {
	if (!array || !invalue || index < 0) return;
	
	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
	if (index >= head[STDX_ARRAY_CAPACITY_FIELD] || (head[STDX_ARRAY_COUNT_FIELD] + 1) > head[STDX_ARRAY_CAPACITY_FIELD]) return;

	u32 offset = (head[STDX_ARRAY_STRIDE_FIELD] * index);

	void* slot = ((u8*)array + offset);
    memcpy(slot, invalue, head[STDX_ARRAY_STRIDE_FIELD]);

	head[STDX_ARRAY_COUNT_FIELD]++;
}

void _writeout_array_impl(void* array, u32 index, void* outvalue) {
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

    structs_api->writein_array(array, index, invalue);
}

void _pop_array_impl(void* array, void* outvalue) {
    if (!array || outvalue == NULL) return;
	
	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
    if (!head[STDX_ARRAY_COUNT_FIELD]) return;

    u32 index = (head[STDX_ARRAY_COUNT_FIELD] - 1);

    structs_api->writeout_array(array, index, outvalue);
}

void* _resize_array_impl(void* array, u32 capacity) {
    if (!array || !capacity) array;

	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
    
    u32 count = head[STDX_ARRAY_COUNT_FIELD];
    u32 stride = head[STDX_ARRAY_STRIDE_FIELD];
    u32 newsize = STDX_ARRAY_HEAD_SIZE + (stride * capacity);

    u32* newhead = (u32*)memory_api->realloc(head, newsize, 16);
    if (!newhead) return array;

    newhead[STDX_ARRAY_SIZE_FIELD] = STDX_ARRAY_HEAD_SIZE + (stride * capacity);
    newhead[STDX_ARRAY_STRIDE_FIELD] = stride;
    newhead[STDX_ARRAY_COUNT_FIELD] = count;
    newhead[STDX_ARRAY_CAPACITY_FIELD] = capacity;

    return (void*)((u8*)newhead + STDX_ARRAY_HEAD_SIZE);
}

void _destroy_array_impl(void* array) {
    memory_api->dealloc((void*)((u8*)array - STDX_ARRAY_HEAD_SIZE));
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

u8 stdx_init_structs(void) {
    structs_api = (_stdx_structs_api*)memory_api->alloc(sizeof(_stdx_structs_api), 16);
    if (!structs_api) return 0;

    structs_api->destroy_array = _destroy_array_impl;
    structs_api->push_array = _push_array_impl;
    structs_api->pop_array = _pop_array_impl;
    structs_api->create_array = _create_array_impl;
    structs_api->resize_array = _resize_array_impl;
    structs_api->writein_array = _writein_array_impl;
    structs_api->writeout_array = _writeout_array_impl;
    
    structs_api->get_array_head = _get_array_head_impl;

    return 1;
}

void stdx_cleanup_structs(void) {
    memory_api->dealloc(structs_api);
}

