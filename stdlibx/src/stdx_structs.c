#include "../include/stdx_structs.h"

#include "stdx_memory.h"

_stdx_structs_api* structs_api = NULL;

/* ---------------- ARRAY ---------------- */
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

void _put_array_impl(void* array, u32 index, void* invalue) {
	if (!array || !invalue || index < 0) return;
	
	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
	if (index >= head[STDX_ARRAY_CAPACITY_FIELD] || (head[STDX_ARRAY_COUNT_FIELD] + 1) > head[STDX_ARRAY_CAPACITY_FIELD]) return;

	u32 offset = (head[STDX_ARRAY_STRIDE_FIELD] * index);

	void* slot = ((u8*)array + offset);
    memcpy(slot, invalue, head[STDX_ARRAY_STRIDE_FIELD]);

	head[STDX_ARRAY_COUNT_FIELD]++;
}

void _pull_array_impl(void* array, u32 index, void* outvalue) {
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

    structs_api->put_array(array, index, invalue);
}

void _pop_array_impl(void* array, void* outvalue) {
    if (!array || outvalue == NULL) return;
	
	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
    if (!head[STDX_ARRAY_COUNT_FIELD]) return;

    u32 index = (head[STDX_ARRAY_COUNT_FIELD] - 1);

    structs_api->pull_array(array, index, outvalue);
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

Array_Head _get_array_head_impl(void* array) {
    Array_Head arrhead = {0, 0, 0, 0};
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
/* ---------------- ARRAY ---------------- */


/* ---------------- LINKED ARRAY ---------------- */
/* ---------------- LINKED ARRAY ---------------- */


/* ---------------- HASHMAP ---------------- */
i32 _fnv1a_hash(cstr string) {
    if (!string) return -1;
    u32 hash = 2166136261u; // FNV-1a offset basis
    while (*string) {
        hash ^= (u8)*string++;  // XOR with current character
        hash *= 16777619;       // multiply by FNV-1a prime
    }; return hash;
}

i32 _probe_key_hashmap(Hashmap* hashmap, cstr key) {
    if (!hashmap || !key) return -1;

    u32 index = _fnv1a_hash(key) % hashmap->max;
    u32 start = index;

    // forward linear probing
    while (
        hashmap->keys[index] != -1 &&
        hashmap->keys[index] != STDX_NOTHING &&
        strcmp(hashmap->keys[index], key) != 0) {
        index = (index + 1) % hashmap->max;  // wraparound
        if (index == start) return -1;
    }
    
    return index;
}

i32 _probe_slot_hashmap(Hashmap* hashmap, cstr key) {
    if (!hashmap || !key) return -1;
    
    i32 index = _fnv1a_hash(key) % hashmap->max;
    i32 start = index;

    while (hashmap->keys[index] != -1 && hashmap->keys[index] != STDX_NOTHING) {
        index = (index + 1) % hashmap->max;
        if (index == start) return -1;
    }

    return index;
}

Hashmap* _create_hashmap_impl(u32 max) {
    Hashmap* hashmap = memory_api->alloc(sizeof(Hashmap), 16);
    if (!hashmap) return NULL;  // error: out of memory!

    hashmap->keys = (cstr*)structs_api->create_array(sizeof(cstr), max);
    memset(hashmap->keys, -1, sizeof(cstr) * max);  // memset to a sentinnel value
    if (!hashmap->keys) {   // error: out of memory!
        memory_api->dealloc(hashmap);
        return NULL;
    }
    
    hashmap->values = (void**)structs_api->create_array(sizeof(void*), max);
    memset(hashmap->values, -1, sizeof(void*) * max);   // memset to a sentinnel value
    if (!hashmap->values) { // error: out of memory!
        structs_api->destroy_array(hashmap->keys);
        memory_api->dealloc(hashmap);
        return NULL;
    }

    hashmap->count = 0;
    hashmap->max = max;
    return hashmap;
}

u8 _set_hashmap_impl(Hashmap* hashmap, cstr key, void* value) {
    if (!hashmap || !key || !value) return STDX_FALSE;  // error: null ptr!

    // 70% load-factor resize
    if (hashmap->count >= (u32)(hashmap->max * 0.7)) {
        hashmap->keys = structs_api->resize_array(hashmap->keys, hashmap->max * 2);
        hashmap->values = structs_api->resize_array(hashmap->values, hashmap->max * 2);
    }

    i32 index = _probe_slot_hashmap(hashmap, key);
    if (index == -1) return STDX_FALSE;   // error: slot not found for key!
    
    hashmap->keys[index] = strdup(key);
    hashmap->values[index] = value;
    hashmap->count++;
    
    return STDX_TRUE;
}

void* _get_hashmap_impl(Hashmap* hashmap, cstr key) {
    if (!hashmap || !key) return STDX_FALSE;  // error: null ptr!
    
    i32 index = _probe_key_hashmap(hashmap, key);
    if (index == -1) return NULL;   // error: key not found!
    
    return hashmap->values[index];
}

u8 _rem_hashmap_impl(Hashmap* hashmap, cstr key) {
    if (!hashmap || !key) return STDX_FALSE;  // error: null ptr!
    
    i32 index = _probe_key_hashmap(hashmap, key);
    if (index == -1) return STDX_FALSE;   // error: key not found!
    
    free((void*)hashmap->keys[index]);
    hashmap->keys[index] = STDX_NOTHING;
    hashmap->values[index] = NULL;
    hashmap->count--;

    return STDX_TRUE;
}

void _destroy_hashmap_impl(Hashmap* hashmap) {
    if (!hashmap) return;   // error: null ptr!
    structs_api->destroy_array(hashmap->values);
    structs_api->destroy_array(hashmap->keys);
    memory_api->dealloc(hashmap);
}
/* ---------------- HASHMAP ---------------- */


/* ---------------- QUAD TREE ---------------- */
/* ---------------- QUAD TREE ---------------- */


/* ---------------- API ---------------- */
u8 stdx_init_structs(void) {
    structs_api = (_stdx_structs_api*)memory_api->alloc(sizeof(_stdx_structs_api), 16);
    if (!structs_api) return 0;

    structs_api->create_array = _create_array_impl;
    structs_api->pop_array = _pop_array_impl;
    structs_api->put_array = _put_array_impl;
    structs_api->push_array = _push_array_impl;
    structs_api->pull_array = _pull_array_impl;
    structs_api->resize_array = _resize_array_impl;
    structs_api->destroy_array = _destroy_array_impl;
    structs_api->get_array_head = _get_array_head_impl;

    structs_api->create_hashmap = _create_hashmap_impl;
    structs_api->set_hashmap = _set_hashmap_impl;
    structs_api->get_hashmap = _get_hashmap_impl;
    structs_api->rem_hashmap = _rem_hashmap_impl;
    structs_api->destroy_hashmap = _destroy_hashmap_impl;

    return 1;
}

void stdx_cleanup_structs(void) {
    memory_api->dealloc(structs_api);
}
/* ---------------- API ---------------- */
