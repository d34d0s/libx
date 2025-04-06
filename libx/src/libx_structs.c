#include "../include/libx_memory.h"
#include "../include/libx_structs.h"

_libx_structs_api* structx = NULL;

/* ---------------- ARRAY ---------------- */
#define LIBX_ARRAY_HEAD_SIZE (sizeof(u32) * 4)

void* _create_array_impl(u32 stride, u32 max) {
    u32* head = (u32*)memx->alloc(LIBX_ARRAY_HEAD_SIZE + (stride * max), 8);
    if (!head) return NULL; // error: out of memory!

    head[LIBX_ARRAY_SIZE_FIELD] = LIBX_ARRAY_HEAD_SIZE + (stride * max);
    head[LIBX_ARRAY_STRIDE_FIELD] = stride;
    head[LIBX_ARRAY_COUNT_FIELD] = 0;
    head[LIBX_ARRAY_MAX_FIELD] = max;

    return (void*)((u8*)head + LIBX_ARRAY_HEAD_SIZE);
}

void _put_array_impl(void* array, u32 index, void* invalue) {
    if (!array || !invalue || index < 0) return;
	
    u32* head = (u32*)((u8*)array - LIBX_ARRAY_HEAD_SIZE);
    if (index >= head[LIBX_ARRAY_MAX_FIELD] || (head[LIBX_ARRAY_COUNT_FIELD] + 1) > head[LIBX_ARRAY_MAX_FIELD]) return;

    u32 offset = (head[LIBX_ARRAY_STRIDE_FIELD] * index);

    void* slot = ((u8*)array + offset);
    memcpy(slot, invalue, head[LIBX_ARRAY_STRIDE_FIELD]);

    head[LIBX_ARRAY_COUNT_FIELD]++;
}

void _pull_array_impl(void* array, u32 index, void* outvalue) {
    if (!array || outvalue == NULL) return;
	
    u32* head = (u32*)((u8*)array - LIBX_ARRAY_HEAD_SIZE);
    if (index >= head[LIBX_ARRAY_MAX_FIELD] || (head[LIBX_ARRAY_COUNT_FIELD] - 1) < 0) return;
    
    u32 stride = head[LIBX_ARRAY_STRIDE_FIELD];
    u32 count = head[LIBX_ARRAY_COUNT_FIELD];
    u32 offset = (stride * index);

    void* slot = ((u8*)array + offset);
    memcpy(outvalue, slot, stride);
    
    // shift array down
    if (index < count) {
        memmove(slot, (u8*)slot + stride, stride * (count - index));
        memset((u8*)slot + stride * (count - index), 0, stride);  // zero the last slot
    }
    
    head[LIBX_ARRAY_COUNT_FIELD]--;
}

void _push_array_impl(void* array, void* invalue) {
    if (!array || !invalue) return;
	
	u32* head = (u32*)((u8*)array - LIBX_ARRAY_HEAD_SIZE);
    u32 index = head[LIBX_ARRAY_COUNT_FIELD];

    structx->array.put_array(array, index, invalue);
}

void _pop_array_impl(void* array, void* outvalue) {
    if (!array || outvalue == NULL) return;
	
	u32* head = (u32*)((u8*)array - LIBX_ARRAY_HEAD_SIZE);
    if (!head[LIBX_ARRAY_COUNT_FIELD]) return;

    u32 index = (head[LIBX_ARRAY_COUNT_FIELD] - 1);

    structx->array.pull_array(array, index, outvalue);
}

void* _resize_array_impl(void* array, u32 max) {
    if (!array || !max) return NULL;
    
	u32* head = (u32*)((u8*)array - LIBX_ARRAY_HEAD_SIZE);
    
    u32 count = head[LIBX_ARRAY_COUNT_FIELD];
    u32 stride = head[LIBX_ARRAY_STRIDE_FIELD];
    u32 newsize = LIBX_ARRAY_HEAD_SIZE + (stride * max);

    u32* newhead = (u32*)memx->realloc(head, newsize, 8);
    if (!newhead) return NULL;

    newhead[LIBX_ARRAY_SIZE_FIELD] = LIBX_ARRAY_HEAD_SIZE + (stride * max);
    newhead[LIBX_ARRAY_STRIDE_FIELD] = stride;
    newhead[LIBX_ARRAY_COUNT_FIELD] = count;
    newhead[LIBX_ARRAY_MAX_FIELD] = max;

    return (void*)((u8*)newhead + LIBX_ARRAY_HEAD_SIZE);
}

void _destroy_array_impl(void* array) {
    memx->dealloc((void*)((u8*)array - LIBX_ARRAY_HEAD_SIZE));
}

Array_Head _get_array_head_impl(void* array) {
    Array_Head arrhead = {0, 0, 0, 0};
    if (!array) return arrhead; // error: null ptr!

    u32* head = (u32*)((u8*)array - LIBX_ARRAY_HEAD_SIZE);
    
    arrhead.size = head[LIBX_ARRAY_SIZE_FIELD];
    arrhead.stride = head[LIBX_ARRAY_STRIDE_FIELD];
    arrhead.count = head[LIBX_ARRAY_COUNT_FIELD];
    arrhead.max = head[LIBX_ARRAY_MAX_FIELD];

    return arrhead;
}

void _set_array_head_impl(void* array, u32 field, u32 value) {
    if (!array || field >= LIBX_ARRAY_FIELDS) return;
    u32* head = (u32*)((u8*)array - LIBX_ARRAY_HEAD_SIZE);
    head[field] = value;
}
/* ---------------- ARRAY ---------------- */


/* ---------------- LINKED ARRAY ---------------- */
Linked_Array* _create_linked_array_impl(Linked_Array* array, u32 stride, u32 max) {
    if (!stride || !max) return NULL;   // error: value error!

    Linked_Array* new = (Linked_Array*)memx->alloc(sizeof(Linked_Array), 8);
    if (!new) return NULL;  // error: out of memory!

    new->array = structx->array.create_array(stride, max);
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

    new->meta = structx->array.get_array_head(new->array);
    return new;
}

void _destroy_linked_array_impl(Linked_Array* array) {
    if (!array) return; // error: null ptr!
    
    if (array->next) array->next->last = array->last;
    if (array->last) array->last->next = array->next;
    
    structx->array.destroy_array(array->array);
    memx->dealloc(array);
    
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
        structx->array.destroy_linked_array(next);
        next = temp;
    }

	Linked_Array* last = array->last;
    while (last) {
        Linked_Array* temp = last->last;
        structx->array.destroy_linked_array(last);
        last = temp;
    }
    
	structx->array.destroy_linked_array(array);
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

    Array_Head meta = structx->array.get_array_head(array->map);

    u32 index = _fnv1a_hash(key) % meta.max;
    u32 start = index;

    LIBX_FORI(0, meta.max, 1) {
        if (array->map[index].key)
            if (strcmp(array->map[index].key, key) == 0) return index;
        index = (index + 1) % meta.max;
        if (index == start) return -1;
    }
    return index;
}

i32 _probe_slot_hash_array(Hash_Array* array, cstr key) {
    if (!array || !key) return -1;
    
    Array_Head meta = structx->array.get_array_head(array->map);
    
    i32 index = _fnv1a_hash(key) % meta.max;
    i32 start = index;
    
    LIBX_FORI(0, meta.max, 1) {
        if (array->map[index].key) {
            if (strcmp(array->map[index].key, key) == 0) return index;
        } else return index;
        index = (index + 1) % meta.max;
        if (index == start) return -1;
    }
    return index;
}

Hash_Array* _create_hash_array_impl(u32 max) {
    Hash_Array* array = (Hash_Array*)memx->alloc(sizeof(Hash_Array), 8);
    if (!array) return NULL;  // error: out of memory!

    array->map = (Key_Value*)structx->array.create_array(sizeof(Key_Value), max);
    if (!array->map) {   // error: out of memory!
        memx->dealloc(array);
        return NULL;
    }

    LIBX_FORI(0, max, 1) {
        array->map[i] = (Key_Value){
            .value = NULL,
            .key = NULL
        };
    }

    array->meta = structx->array.get_array_head(array->map);
    return array;
}

u8 _put_hash_array_impl(Hash_Array* array, cstr key, void* value) {
    if (!array || !key || !value) return LIBX_FALSE;  // error: null ptr!

    Array_Head meta = structx->array.get_array_head(array->map);
    if (meta.count > (u32)(meta.max * 0.7)) {
        Key_Value* temp = (Key_Value*)structx->array.create_array(sizeof(Key_Value), meta.max * 2);
        if (!temp) return LIBX_FALSE;  // error: out of memory!

        ((u32*)((u8*)temp - LIBX_ARRAY_HEAD_SIZE))[LIBX_ARRAY_COUNT_FIELD] = meta.count;
        memcpy(temp, array->map, meta.stride * meta.max);
        
        structx->array.destroy_array(array->map);
        array->map = temp;
    }
    
    i32 index = _probe_slot_hash_array(array, key);
    if (index == -1) return LIBX_FALSE;

    if (array->map[index].key && strcmp(array->map[index].key, key) == 0) {
        array->map[index].value = value;
    } else {
        structx->array.put_array(array->map, index, &(Key_Value){ .value = value, .key = strdup(key)});
    }
    
    array->meta = structx->array.get_array_head(array->map);
    return LIBX_TRUE;
}

void* _get_hash_array_impl(Hash_Array* array, cstr key) {
    if (!array || !key) return NULL;  // error: null ptr!
    
    i32 index = _probe_key_hash_array(array, key);
    if (index == -1) return NULL;

    return array->map[index].value;
}

u8 _pull_hash_array_impl(Hash_Array* array, cstr key, Key_Value* out) {
    if (!array || !key) return LIBX_FALSE;  // error: null ptr!

    i32 index = _probe_key_hash_array(array, key);
    if (index == -1) return LIBX_FALSE;

    structx->array.pull_array(array->map, index, out);
    array->meta = structx->array.get_array_head(array->map);
    return LIBX_TRUE;
}

void _destroy_hash_array_impl(Hash_Array* array) {
    if (!array) return;   // error: null ptr!
    structx->array.destroy_array(array->map);
    memx->dealloc(array);
}

cstr* _get_hash_array_keys_impl(Hash_Array* array) {
    u8 key = 0;
	
    cstr* keys = structx->array.create_array(sizeof(cstr), array->meta.count);
    if (!keys) return NULL; // error: out of memory!

	LIBX_FORI(0, array->meta.max, 1)
		if (array->map[i].key)
            keys[key++] = array->map[i].key;
	
    ((u32*)((u8*)keys - LIBX_ARRAY_HEAD_SIZE))[LIBX_ARRAY_COUNT_FIELD] = array->meta.count;
    return keys;
}

void** _get_hash_array_values_impl(Hash_Array* array) {
    u8 value = 0;
	
    void** values = structx->array.create_array(sizeof(void*), array->meta.count);
    if (!values) return NULL; // error: out of memory!

	LIBX_FORI(0, array->meta.max, 1)
		if (array->map[i].key)
            values[value++] = array->map[i].value;
	
    ((u32*)((u8*)values - LIBX_ARRAY_HEAD_SIZE))[LIBX_ARRAY_COUNT_FIELD] = array->meta.count;
    return values;
}
/* ---------------- HASH ARRAY ---------------- */


/* ---------------- API ---------------- */
u8 libx_init_structs(void) {
    if (structx != NULL) return LIBX_TRUE;    // redundant call: struct API already initialized!

    if (!memx) {
        printf("libx memory api not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize memory api!
    }

    structx = (_libx_structs_api*)memx->alloc(sizeof(_libx_structs_api), 8);
    if (!structx) return 0;

    structx->array.create_array = _create_array_impl;
    structx->array.pop_array = _pop_array_impl;
    structx->array.put_array = _put_array_impl;
    structx->array.push_array = _push_array_impl;
    structx->array.pull_array = _pull_array_impl;
    structx->array.resize_array = _resize_array_impl;
    structx->array.destroy_array = _destroy_array_impl;
    structx->array.get_array_head = _get_array_head_impl;

    structx->array.create_linked_array = _create_linked_array_impl;
    structx->array.destroy_linked_array = _destroy_linked_array_impl;
    structx->array.collapse_linked_array = _collapse_linked_array_impl;

    structx->array.create_hash_array = _create_hash_array_impl;
    structx->array.put_hash_array = _put_hash_array_impl;
    structx->array.get_hash_array = _get_hash_array_impl;
    structx->array.get_hash_array_keys = _get_hash_array_keys_impl;
    structx->array.get_hash_array_values = _get_hash_array_values_impl;
    structx->array.pull_hash_array = _pull_hash_array_impl;
    structx->array.destroy_hash_array = _destroy_hash_array_impl;
    
    return 1;
}

void libx_cleanup_structs(void) {
    if (structx == NULL) return;    // error: struct API not initialized!
    memx->dealloc(structx);
    structx = NULL;
}
/* ---------------- API ---------------- */
