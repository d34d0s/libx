#include <corex/corex.h>

/* ---------------- ARRAY ---------------- */
#define ARRAY_HEAD_SIZE (sizeof(u32) * 4)

void* _create_array_impl(u32 stride, u32 max) {
    u32* head = (u32*)corex->memx.alloc(ARRAY_HEAD_SIZE + (stride * max), 8);
    if (!head) return NULL; // error: out of memory!

    head[ARRAY_SIZE_FIELD] = ARRAY_HEAD_SIZE + (stride * max);
    head[ARRAY_STRIDE_FIELD] = stride;
    head[ARRAY_COUNT_FIELD] = 0;
    head[ARRAY_MAX_FIELD] = max;

    return (void*)((byte*)head + ARRAY_HEAD_SIZE);
}

void _put_array_impl(void* array, u32 index, void* invalue) {
    if (!array || !invalue || index < 0) return;
	
    u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    if (index >= head[ARRAY_MAX_FIELD] || (head[ARRAY_COUNT_FIELD] + 1) > head[ARRAY_MAX_FIELD]) return;

    u32 offset = (head[ARRAY_STRIDE_FIELD] * index);

    void* slot = ((byte*)array + offset);
    memcpy(slot, invalue, head[ARRAY_STRIDE_FIELD]);

    head[ARRAY_COUNT_FIELD]++;
}

void _pull_array_impl(void* array, u32 index, void* outvalue) {
    if (!array || outvalue == NULL) return;
	
    u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    if (index >= head[ARRAY_MAX_FIELD] || (head[ARRAY_COUNT_FIELD] - 1) < 0) return;
    
    u32 stride = head[ARRAY_STRIDE_FIELD];
    u32 count = head[ARRAY_COUNT_FIELD];
    u32 offset = (stride * index);

    void* slot = ((byte*)array + offset);
    memcpy(outvalue, slot, stride);
    
    // shift array down
    if (index < count) {
        memmove(slot, (byte*)slot + stride, stride * (count - index));
        memset((byte*)slot + stride * (count - index), 0, stride);  // zero the last slot
    }
    
    head[ARRAY_COUNT_FIELD]--;
}

void _push_array_impl(void* array, void* invalue) {
    if (!array || !invalue) return;
	
	u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    u32 index = head[ARRAY_COUNT_FIELD];

    corex->dsx.array.put_array(array, index, invalue);
}

void _pop_array_impl(void* array, void* outvalue) {
    if (!array || outvalue == NULL) return;
	
	u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    if (!head[ARRAY_COUNT_FIELD]) return;

    u32 index = (head[ARRAY_COUNT_FIELD] - 1);

    corex->dsx.array.pull_array(array, index, outvalue);
}

void* _resize_array_impl(void* array, u32 max) {
    if (!array || !max) return NULL;
    
	u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    
    u32 count = head[ARRAY_COUNT_FIELD];
    u32 stride = head[ARRAY_STRIDE_FIELD];
    u32 newsize = ARRAY_HEAD_SIZE + (stride * max);

    u32* newhead = (u32*)corex->memx.realloc(head, newsize, 8);
    if (!newhead) return NULL;

    newhead[ARRAY_SIZE_FIELD] = ARRAY_HEAD_SIZE + (stride * max);
    newhead[ARRAY_STRIDE_FIELD] = stride;
    newhead[ARRAY_COUNT_FIELD] = count;
    newhead[ARRAY_MAX_FIELD] = max;

    return (void*)((byte*)newhead + ARRAY_HEAD_SIZE);
}

void _destroy_array_impl(void* array) {
    corex->memx.dealloc((void*)((byte*)array - ARRAY_HEAD_SIZE));
}

Array_Head _get_array_head_impl(void* array) {
    Array_Head arrhead = {0, 0, 0, 0};
    if (!array) return arrhead; // error: null ptr!

    u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    
    arrhead.size = head[ARRAY_SIZE_FIELD];
    arrhead.stride = head[ARRAY_STRIDE_FIELD];
    arrhead.count = head[ARRAY_COUNT_FIELD];
    arrhead.max = head[ARRAY_MAX_FIELD];

    return arrhead;
}

void _set_array_head_impl(void* array, u32 field, u32 value) {
    if (!array || field >= ARRAY_FIELDS) return;
    u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    head[field] = value;
}
/* ---------------- ARRAY ---------------- */


/* ---------------- LINKED ARRAY ---------------- */
Linked_Array* _create_linked_array_impl(Linked_Array* array, u32 stride, u32 max) {
    if (!stride || !max) return NULL;   // error: value error!

    Linked_Array* new = (Linked_Array*)corex->memx.alloc(sizeof(Linked_Array), 8);
    if (!new) return NULL;  // error: out of memory!

    new->array = corex->dsx.array.create_array(stride, max);
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

    new->meta = corex->dsx.array.get_array_head(new->array);
    return new;
}

void _destroy_linked_array_impl(Linked_Array* array) {
    if (!array) return; // error: null ptr!
    
    if (array->next) array->next->last = array->last;
    if (array->last) array->last->next = array->next;
    
    corex->dsx.array.destroy_array(array->array);
    corex->memx.dealloc(array);
    
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
        corex->dsx.array.destroy_linked_array(next);
        next = temp;
    }

	Linked_Array* last = array->last;
    while (last) {
        Linked_Array* temp = last->last;
        corex->dsx.array.destroy_linked_array(last);
        last = temp;
    }
    
	corex->dsx.array.destroy_linked_array(array);
}
/* ---------------- LINKED ARRAY ---------------- */


/* ---------------- HASH ARRAY ---------------- */
i32 _fnv1a_hash(cstr string) {
    if (!string) return -1;
    u32 hash = 2166136261u; // FNV-1a offset basis
    while (*string) {
        hash ^= (byte)*string++;  // XOR with current character
        hash *= 16777619;       // multiply by FNV-1a prime
    }; return hash;
}

i32 _probe_key_hash_array(Hash_Array* array, cstr key) {
    if (!array || !key) return -1;

    Array_Head meta = corex->dsx.array.get_array_head(array->map);

    u32 index = _fnv1a_hash(key) % meta.max;
    u32 start = index;

    COREX_FORI(0, meta.max, 1) {
        if (array->map[index].key)
            if (strcmp(array->map[index].key, key) == 0) return index;
        index = (index + 1) % meta.max;
        if (index == start) return -1;
    }
    return index;
}

i32 _probe_slot_hash_array(Hash_Array* array, cstr key) {
    if (!array || !key) return -1;
    
    Array_Head meta = corex->dsx.array.get_array_head(array->map);
    
    i32 index = _fnv1a_hash(key) % meta.max;
    i32 start = index;
    
    COREX_FORI(0, meta.max, 1) {
        if (array->map[index].key) {
            if (strcmp(array->map[index].key, key) == 0) return index;
        } else return index;
        index = (index + 1) % meta.max;
        if (index == start) return -1;
    }
    return index;
}

Hash_Array* _create_hash_array_impl(u32 max) {
    Hash_Array* array = (Hash_Array*)corex->memx.alloc(sizeof(Hash_Array), 8);
    if (!array) return NULL;  // error: out of memory!

    array->map = (Key_Value*)corex->dsx.array.create_array(sizeof(Key_Value), max);
    if (!array->map) {   // error: out of memory!
        corex->memx.dealloc(array);
        return NULL;
    }

    COREX_FORI(0, max, 1) {
        array->map[i] = (Key_Value){
            .value = NULL,
            .key = NULL
        };
    }

    array->meta = corex->dsx.array.get_array_head(array->map);
    return array;
}

byte _put_hash_array_impl(Hash_Array* array, cstr key, void* value) {
    if (!array || !key || !value) return COREX_FALSE;  // error: null ptr!

    Array_Head meta = corex->dsx.array.get_array_head(array->map);
    if (meta.count > (u32)(meta.max * 0.7)) {
        Key_Value* temp = (Key_Value*)corex->dsx.array.create_array(sizeof(Key_Value), meta.max * 2);
        if (!temp) return COREX_FALSE;  // error: out of memory!

        ((u32*)((byte*)temp - ARRAY_HEAD_SIZE))[ARRAY_COUNT_FIELD] = meta.count;
        memcpy(temp, array->map, meta.stride * meta.max);
        
        corex->dsx.array.destroy_array(array->map);
        array->map = temp;
    }
    
    i32 index = _probe_slot_hash_array(array, key);
    if (index == -1) return COREX_FALSE;

    if (array->map[index].key && strcmp(array->map[index].key, key) == 0) {
        array->map[index].value = value;
    } else {
        corex->dsx.array.put_array(array->map, index, &(Key_Value){ .value = value, .key = strdup(key)});
    }
    
    array->meta = corex->dsx.array.get_array_head(array->map);
    return COREX_TRUE;
}

void* _get_hash_array_impl(Hash_Array* array, cstr key) {
    if (!array || !key) return NULL;  // error: null ptr!
    
    i32 index = _probe_key_hash_array(array, key);
    if (index == -1) return NULL;

    return array->map[index].value;
}

byte _pull_hash_array_impl(Hash_Array* array, cstr key, Key_Value* out) {
    if (!array || !key) return COREX_FALSE;  // error: null ptr!

    i32 index = _probe_key_hash_array(array, key);
    if (index == -1) return COREX_FALSE;

    corex->dsx.array.pull_array(array->map, index, out);
    array->meta = corex->dsx.array.get_array_head(array->map);
    return COREX_TRUE;
}

void _destroy_hash_array_impl(Hash_Array* array) {
    if (!array) return;   // error: null ptr!
    corex->dsx.array.destroy_array(array->map);
    corex->memx.dealloc(array);
}

cstr* _get_hash_array_keys_impl(Hash_Array* array) {
    u8 key = 0;
	
    cstr* keys = corex->dsx.array.create_array(sizeof(cstr), array->meta.count);
    if (!keys) return NULL; // error: out of memory!

	COREX_FORI(0, array->meta.max, 1)
		if (array->map[i].key)
            keys[key++] = array->map[i].key;
	
    ((u32*)((byte*)keys - ARRAY_HEAD_SIZE))[ARRAY_COUNT_FIELD] = array->meta.count;
    return keys;
}

void** _get_hash_array_values_impl(Hash_Array* array) {
    u8 value = 0;
	
    void** values = corex->dsx.array.create_array(sizeof(void*), array->meta.count);
    if (!values) return NULL; // error: out of memory!

	COREX_FORI(0, array->meta.max, 1)
		if (array->map[i].key)
            values[value++] = array->map[i].value;
	
    ((u32*)((byte*)values - ARRAY_HEAD_SIZE))[ARRAY_COUNT_FIELD] = array->meta.count;
    return values;
}
/* ---------------- HASH ARRAY ---------------- */


/* ---------------- API ---------------- */
byte _corex_init_dsx(void) {
    if (!corex) return COREX_FALSE; // error: null ptr!
    if (corex->dsx.init == COREX_TRUE) return COREX_TRUE;    // redundant call: Dsx API already initialized!

    if (corex->memx.init == COREX_FALSE) {
        printf("Corex Memx api not initialized!\n");
        return COREX_FALSE; // error: failed to initialize Memx API!
    }

    corex->dsx.array.create_array = _create_array_impl;
    corex->dsx.array.pop_array = _pop_array_impl;
    corex->dsx.array.put_array = _put_array_impl;
    corex->dsx.array.push_array = _push_array_impl;
    corex->dsx.array.pull_array = _pull_array_impl;
    corex->dsx.array.resize_array = _resize_array_impl;
    corex->dsx.array.destroy_array = _destroy_array_impl;
    corex->dsx.array.get_array_head = _get_array_head_impl;

    corex->dsx.array.create_linked_array = _create_linked_array_impl;
    corex->dsx.array.destroy_linked_array = _destroy_linked_array_impl;
    corex->dsx.array.collapse_linked_array = _collapse_linked_array_impl;

    corex->dsx.array.create_hash_array = _create_hash_array_impl;
    corex->dsx.array.put_hash_array = _put_hash_array_impl;
    corex->dsx.array.get_hash_array = _get_hash_array_impl;
    corex->dsx.array.get_hash_array_keys = _get_hash_array_keys_impl;
    corex->dsx.array.get_hash_array_values = _get_hash_array_values_impl;
    corex->dsx.array.pull_hash_array = _pull_hash_array_impl;
    corex->dsx.array.destroy_hash_array = _destroy_hash_array_impl;
    
    corex->meta.usage.dsx = sizeof(Dsx);
	corex->dsx.init = COREX_TRUE;
    return 1;
}

void _corex_cleanup_dsx(void) {
    if (corex->dsx.init == COREX_FALSE) return;    // error: Dsx API not initialized!
	corex->meta.usage.apis &= ~COREX_DSX;
    corex->dsx.init = COREX_FALSE;
    corex->meta.usage.dsx = 0;
    corex->dsx = (Dsx){0};
}
/* ---------------- API ---------------- */
