#include "../include/stdx_structs.h"

#include "stdx_memory.h"

_stdx_structs_api* structs_api = NULL;

/* ---------------- ARRAY ---------------- */
#define STDX_ARRAY_HEAD_SIZE (sizeof(u32) * 4)

void* _create_array_impl(u32 stride, u32 max) {
    u32* head = (u32*)memory_api->alloc(STDX_ARRAY_HEAD_SIZE + (stride * max), 16);
    if (!head) return NULL; // error: out of memory!

    head[STDX_ARRAY_SIZE_FIELD] = STDX_ARRAY_HEAD_SIZE + (stride * max);
    head[STDX_ARRAY_STRIDE_FIELD] = stride;
    head[STDX_ARRAY_COUNT_FIELD] = 0;
    head[STDX_ARRAY_MAX_FIELD] = max;

    return (void*)((u8*)head + STDX_ARRAY_HEAD_SIZE);
}

void _put_array_impl(void* array, u32 index, void* invalue) {
	if (!array || !invalue || index < 0) return;
	
	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
	if (index >= head[STDX_ARRAY_MAX_FIELD] || (head[STDX_ARRAY_COUNT_FIELD] + 1) > head[STDX_ARRAY_MAX_FIELD]) return;

	u32 offset = (head[STDX_ARRAY_STRIDE_FIELD] * index);

	void* slot = ((u8*)array + offset);
    memcpy(slot, invalue, head[STDX_ARRAY_STRIDE_FIELD]);

	head[STDX_ARRAY_COUNT_FIELD]++;
}

void _pull_array_impl(void* array, u32 index, void* outvalue) {
    if (!array || outvalue == NULL) return;
	
	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
	if (index >= head[STDX_ARRAY_MAX_FIELD] || (head[STDX_ARRAY_COUNT_FIELD] - 1) < 0) return;

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

void* _resize_array_impl(void* array, u32 max) {
    if (!array || !max) array;

	u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
    
    u32 count = head[STDX_ARRAY_COUNT_FIELD];
    u32 stride = head[STDX_ARRAY_STRIDE_FIELD];
    u32 newsize = STDX_ARRAY_HEAD_SIZE + (stride * max);

    u32* newhead = (u32*)memory_api->realloc(head, newsize, 16);
    if (!newhead) return array;

    newhead[STDX_ARRAY_SIZE_FIELD] = STDX_ARRAY_HEAD_SIZE + (stride * max);
    newhead[STDX_ARRAY_STRIDE_FIELD] = stride;
    newhead[STDX_ARRAY_COUNT_FIELD] = count;
    newhead[STDX_ARRAY_MAX_FIELD] = max;

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
    arrhead.max = head[STDX_ARRAY_MAX_FIELD];

    return arrhead;
}

void _set_array_head_impl(void* array, u32 field, u32 value) {
    if (!array || field >= STDX_ARRAY_FIELDS) return;
    u32* head = (u32*)((u8*)array - STDX_ARRAY_HEAD_SIZE);
    head[field] = value;
}
/* ---------------- ARRAY ---------------- */


/* ---------------- LINKED ARRAY ---------------- */
Linked_Array* _create_linked_array_impl(Linked_Array* array, u32 stride, u32 max) {
    if (!stride || !max) return NULL;   // error: value error!

    Linked_Array* new = (Linked_Array*)memory_api->alloc(sizeof(Linked_Array), 16);
    if (!new) return NULL;  // error: out of memory!

    new->array = structs_api->create_array(stride, max);
    if (!new->array) return NULL;    // error: out of memory!

    new->last = NULL;
    new->next = NULL;
    if (array) {
        new->last = array;
        if (array->next) {
            new->next = array->next;
            array->next->last = new;
        }
        array->next = new;
    }

    new->meta = structs_api->get_array_head(new->array);
    return new;
}

void _destroy_linked_array_impl(Linked_Array* array) {
    if (!array) return; // error: null ptr!
    
    if (array->next) array->next->last = array->last;
    if (array->last) array->last->next = array->next;
    
    structs_api->destroy_array(array->array);
    memory_api->dealloc(array);
    
    array->meta = (Array_Head){0};
    array->array = NULL;
    array->last = NULL;
    array->next = NULL;
}

void _collapse_linked_array_impl(Linked_Array* array) {
    if (!array) return; // error: null ptr!
    
    Linked_Array* next = array->next;
    while (next) {
        Linked_Array* temp = next->next;
        structs_api->destroy_linked_array(next);
        next = temp;
    }

	Linked_Array* last = array->last;
    while (last) {
        Linked_Array* temp = last->last;
        structs_api->destroy_linked_array(last);
        last = temp;
    }
    
	structs_api->destroy_linked_array(array);
}
/* ---------------- LINKED ARRAY ---------------- */


/* ---------------- HASH ARRAY ---------------- */
i32 _fnv1a_hash(cstr string) {
    if (!string) return -1;
    u32 hash = 2166136261u; // FNV-1a offset basis
    while (*string) {
        hash ^= (u8)*string++;  // XOR with current character
        hash *= 16777619;       // multiply by FNV-1a prime
    }; return hash;
}

i32 _probe_key_hash_array(Hash_Array* array, cstr key) {
    if (!array || !key) return -1;

    Array_Head meta = structs_api->get_array_head(array->map);

    u32 index = _fnv1a_hash(key) % meta.max;
    u32 start = index;

    STDX_FORI(0, meta.max, 1) {
        if (array->map[index].key && strcmp(array->map[index].key, key) == 0) break;
        else {
            index = (index + 1) % meta.max;
            if (index == start) return -1;
        }
    }

    return index;
}

i32 _probe_slot_hash_array(Hash_Array* array, cstr key) {
    if (!array || !key) return -1;
    
    Array_Head meta = structs_api->get_array_head(array->map);
    
    i32 index = _fnv1a_hash(key) % meta.max;
    i32 start = index;

    while (array->map[index].key) {
        index = (index + 1) % meta.max;
        if (index == start) return -1;
    }
  
    return index;
}

Hash_Array* _create_hash_array_impl(u32 max) {
    Hash_Array* array = memory_api->alloc(sizeof(Hash_Array), 16);
    if (!array) return NULL;  // error: out of memory!

    array->map = structs_api->create_array(sizeof(Key_Value), max);
    if (!array->map) {   // error: out of memory!
        memory_api->dealloc(array);
        return NULL;
    }
    
    array->meta = structs_api->get_array_head(array->map);
    return array;
}

u8 _put_hash_array_impl(Hash_Array* array, cstr key, void* value) {
    if (!array || !key || !value) return STDX_FALSE;  // error: null ptr!

    Array_Head meta = structs_api->get_array_head(array->map);
    if (meta.count > (u32)(meta.max * 0.7)) {
        Key_Value* temp = structs_api->create_array(sizeof(Key_Value), meta.max * 2);
        if (!temp) return STDX_FALSE;  // error: out of memory!

        memcpy(temp, array->map, meta.max * meta.stride);
        structs_api->destroy_array(array->map);
        array->map = temp;
    }

    i32 index = _probe_slot_hash_array(array, key);
    if (index == -1) return -1;
    
    structs_api->put_array(array->map, index, &(Key_Value){ .value = value, .key = strdup(key)});
    
    array->meta = structs_api->get_array_head(array->map);
    return STDX_TRUE;
}

void* _get_hash_array_impl(Hash_Array* array, cstr key) {
    if (!array || !key) return STDX_FALSE;  // error: null ptr!
    
    i32 index = _probe_key_hash_array(array, key);
    if (index == -1) return NULL;

    return array->map[index].value;
}

u8 _pull_hash_array_impl(Hash_Array* array, cstr key, Key_Value* out) {
    if (!array || !key) return STDX_FALSE;  // error: null ptr!

    i32 index = _probe_key_hash_array(array, key);
    if (index == -1) return STDX_FALSE;

    structs_api->pull_array(array->map, index, out);

    array->meta = structs_api->get_array_head(array->map);
    return STDX_TRUE;
}

void _destroy_hash_array_impl(Hash_Array* array) {
    if (!array) return;   // error: null ptr!
    structs_api->destroy_array(array->map);
    memory_api->dealloc(array);
}
/* ---------------- HASH ARRAY ---------------- */


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

    structs_api->create_linked_array = _create_linked_array_impl;
    structs_api->destroy_linked_array = _destroy_linked_array_impl;
    structs_api->collapse_linked_array = _collapse_linked_array_impl;

    structs_api->create_hash_array = _create_hash_array_impl;
    structs_api->put_hash_array = _put_hash_array_impl;
    structs_api->get_hash_array = _get_hash_array_impl;
    structs_api->pull_hash_array = _pull_hash_array_impl;
    structs_api->destroy_hash_array = _destroy_hash_array_impl;
    
    return 1;
}

void stdx_cleanup_structs(void) {
    memory_api->dealloc(structs_api);
}
/* ---------------- API ---------------- */
