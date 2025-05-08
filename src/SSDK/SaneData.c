#include <include/SSDK/SSDK.h>

SaneData* saneData = NULL;

/* ---------------- ARRAY ---------------- */
#define ARRAY_HEAD_SIZE (sizeof(u32) * 4)

ptr _createArrayImpl(u32 stride, u32 max) {
    u32* head = (u32*)saneMemory->alloc(ARRAY_HEAD_SIZE + (stride * max), 8);
    if (!head) return NULL; // error: out of memory!

    head[ARRAY_SIZE_FIELD] = ARRAY_HEAD_SIZE + (stride * max);
    head[ARRAY_STRIDE_FIELD] = stride;
    head[ARRAY_COUNT_FIELD] = 0;
    head[ARRAY_MAX_FIELD] = max;

    return (ptr)((byte*)head + ARRAY_HEAD_SIZE);
}

none _putArrayImpl(ptr array, u32 index, ptr invalue) {
    if (!array || !invalue || index < 0) return;
	
    u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    if (index >= head[ARRAY_MAX_FIELD] || (head[ARRAY_COUNT_FIELD] + 1) > head[ARRAY_MAX_FIELD]) return;

    u32 offset = (head[ARRAY_STRIDE_FIELD] * index);

    ptr slot = ((byte*)array + offset);
    memcpy(slot, invalue, head[ARRAY_STRIDE_FIELD]);

    head[ARRAY_COUNT_FIELD]++;
}

none _pullArrayImpl(ptr array, u32 index, ptr outvalue) {
    if (!array || outvalue == NULL) return;
	
    u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    if (index >= head[ARRAY_MAX_FIELD] || (head[ARRAY_COUNT_FIELD] - 1) < 0) return;
    
    u32 stride = head[ARRAY_STRIDE_FIELD];
    u32 count = head[ARRAY_COUNT_FIELD];
    u32 offset = (stride * index);

    ptr slot = ((byte*)array + offset);
    memcpy(outvalue, slot, stride);
    
    // shift array down
    if (index < count) {
        memmove(slot, (byte*)slot + stride, stride * (count - index));
        memset((byte*)slot + stride * (count - index), 0, stride);  // zero the last slot
    }
    
    head[ARRAY_COUNT_FIELD]--;
}

none _pushArrayImpl(ptr array, ptr invalue) {
    if (!array || !invalue) return;
	
	u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    u32 index = head[ARRAY_COUNT_FIELD];

    saneData->array.put(array, index, invalue);
}

none _popArrayImpl(ptr array, ptr outvalue) {
    if (!array || outvalue == NULL) return;
	
	u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    if (!head[ARRAY_COUNT_FIELD]) return;

    u32 index = (head[ARRAY_COUNT_FIELD] - 1);

    saneData->array.pull(array, index, outvalue);
}

ptr _resizeArrayImpl(ptr array, u32 max) {
    if (!array || !max) return NULL;
    
	u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    
    u32 count = head[ARRAY_COUNT_FIELD];
    u32 stride = head[ARRAY_STRIDE_FIELD];
    u32 newsize = ARRAY_HEAD_SIZE + (stride * max);

    u32* newhead = (u32*)saneMemory->realloc(head, newsize, 8);
    if (!newhead) return NULL;

    newhead[ARRAY_SIZE_FIELD] = ARRAY_HEAD_SIZE + (stride * max);
    newhead[ARRAY_STRIDE_FIELD] = stride;
    newhead[ARRAY_COUNT_FIELD] = count;
    newhead[ARRAY_MAX_FIELD] = max;

    return (ptr)((byte*)newhead + ARRAY_HEAD_SIZE);
}

none _destroyArrayImpl(ptr array) {
    saneMemory->dealloc((ptr)((byte*)array - ARRAY_HEAD_SIZE));
}

ArrayHeader _getHeaderImpl(ptr array) {
    ArrayHeader arrhead = {0, 0, 0, 0};
    if (!array) return arrhead; // error: null ptr!

    u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    
    arrhead.size = head[ARRAY_SIZE_FIELD];
    arrhead.stride = head[ARRAY_STRIDE_FIELD];
    arrhead.count = head[ARRAY_COUNT_FIELD];
    arrhead.max = head[ARRAY_MAX_FIELD];

    return arrhead;
}

none _setArrayHeadImpl(ptr array, u32 field, u32 value) {
    if (!array || field >= ARRAY_FIELDS) return;
    u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    head[field] = value;
}
/* ---------------- ARRAY ---------------- */


/* ---------------- LINKED ARRAY ---------------- */
none _createLinkedArrayImpl(u32 stride, u32 max, LinkedArray* array) {
    if (!array) return;  // error: null ptr!
    if (!stride || !max) return;   // error: value error!

    if (array->array != NULL) {
        array->next = saneMemory->alloc(sizeof(LinkedArray), 8);
        if (!array->next) return;  // error: out of memory!
    
        array->next->array = saneData->array.create(stride, max);
        if (!array->next->array) {
            saneMemory->dealloc(array->next);
            return;    // error: out of memory!
        }

        array->next->next = NULL;
        array->next->last = array;
        array->next->meta = saneData->array.getHeader(array->next->array);
    } else {
        array->array = saneData->array.create(stride, max);
        if (!array->array) return;    // error: out of memory!
        array->next = NULL;
        array->last = NULL;
        array->meta = saneData->array.getHeader(array->array);
    }
}

none _destroyLinkedArrayImpl(LinkedArray* array) {
    if (!array) return; // error: null ptr!
    
    if (array->next) array->next->last = array->last;
    if (array->last) array->last->next = array->next;
    
    saneData->array.destroy(array->array);
    
    array->meta = (ArrayHeader){0};
    array->array = NULL;
    array->last = NULL;
    array->next = NULL;
}

none _collapseLinkedArrayImpl(LinkedArray* array) {
    if (!array) return; // error: null ptr!
    
    LinkedArray* next = array->next;
    while (next) {
        LinkedArray* temp = next->next;
        saneData->array.destroy(next);
        next = temp;
    }

	LinkedArray* last = array->last;
    while (last) {
        LinkedArray* temp = last->last;
        saneData->array.destroy(last);
        last = temp;
    }
    
	saneData->array.destroy(array);
}
/* ---------------- LINKED ARRAY ---------------- */


/* ---------------- HASH ARRAY ---------------- */
i32 _fnv1aHash_(cstr string) {
    if (!string) return -1;
    u32 hash = 2166136261u; // FNV-1a offset basis
    while (*string) {
        hash ^= (byte)*string++;  // XOR with current character
        hash *= 16777619;       // multiply by FNV-1a prime
    }; return hash;
}

i32 _probeKeyHashArray_(cstr key, HashArray* array) {
    if (!array || !key) return -1;

    ArrayHeader meta = saneData->array.getHeader(array->map);

    u32 index = _fnv1aHash_(key) % meta.max;
    u32 start = index;

    SSDK_FORI(0, meta.max, 1) {
        if (array->map[index].key)
            if (strcmp(array->map[index].key, key) == 0) return index;
        index = (index + 1) % meta.max;
        if (index == start) return -1;
    }
    return index;
}

i32 _probeSlotHashArray_(cstr key, HashArray* array) {
    if (!array || !key) return -1;
    
    ArrayHeader meta = saneData->array.getHeader(array->map);
    
    i32 index = _fnv1aHash_(key) % meta.max;
    i32 start = index;
    
    SSDK_FORI(0, meta.max, 1) {
        if (array->map[index].key) {
            if (strcmp(array->map[index].key, key) == 0) return index;
        } else return index;
        index = (index + 1) % meta.max;
        if (index == start) return -1;
    }
    return index;
}

none _createHashArrayImpl(u32 max, HashArray* array) {
    if (!array) return;  // error: out of memory!

    array->map = (KeyValue*)saneData->array.create(sizeof(KeyValue), max);
    if (!array->map) {
        return; // error: out of memory!
    }

    SSDK_FORI(0, max, 1) {
        array->map[i] = (KeyValue){
            .value = NULL,
            .key = NULL
        };
    }

    array->meta = saneData->array.getHeader(array->map);
}

byte _putHashArrayImpl(cstr key, ptr value, HashArray* array) {
    if (!array || !key || !value) return SSDK_FALSE;  // error: null ptr!

    ArrayHeader meta = saneData->array.getHeader(array->map);
    if (meta.count > (u32)(meta.max * 0.7)) {
        KeyValue* temp = (KeyValue*)saneData->array.create(sizeof(KeyValue), meta.max * 2);
        if (!temp) return SSDK_FALSE;  // error: out of memory!

        ((u32*)((byte*)temp - ARRAY_HEAD_SIZE))[ARRAY_COUNT_FIELD] = meta.count;
        memcpy(temp, array->map, meta.stride * meta.max);
        
        saneData->array.destroy(array->map);
        array->map = temp;
    }
    
    i32 index = _probeSlotHashArray_(key, array);
    if (index == -1) return SSDK_FALSE;

    if (array->map[index].key && strcmp(array->map[index].key, key) == 0) {
        array->map[index].value = value;
    } else {
        saneData->array.put(array->map, index, &(KeyValue){ .value = value, .key = strdup(key)});
    }
    
    array->meta = saneData->array.getHeader(array->map);
    return SSDK_TRUE;
}

ptr _getHashArrayImpl(cstr key, HashArray* array) {
    if (!array || !key) return NULL;  // error: null ptr!
    
    i32 index = _probeKeyHashArray_(key, array);
    if (index == -1) return NULL;

    return array->map[index].value;
}

byte _pullHashArrayImpl(cstr key, KeyValue* out, HashArray* array) {
    if (!array || !array->map || !out || !key) return SSDK_FALSE;  // error: null ptr!

    i32 index = _probeKeyHashArray_(key, array);
    if (index == -1) return SSDK_FALSE;
    
    saneData->array.pull(array->map, index, out);
    
    array->meta = saneData->array.getHeader(array->map);
    return SSDK_TRUE;
}

none _destroyHashArrayImpl(HashArray* array) {
    if (!array) return;   // error: null ptr!
    saneData->array.destroy(array->map);
}

cstr* _getHashArrayKeysImpl(HashArray* array) {
    u8 key = 0;
	
    cstr* keys = saneData->array.create(sizeof(cstr), array->meta.count);
    if (!keys) return NULL; // error: out of memory!

	SSDK_FORI(0, array->meta.max, 1)
		if (array->map[i].key)
            keys[key++] = array->map[i].key;
	
    ((u32*)((byte*)keys - ARRAY_HEAD_SIZE))[ARRAY_COUNT_FIELD] = array->meta.count;
    return keys;
}

ptr* _getHashArrayValuesImpl(HashArray* array) {
    u8 value = 0;
	
    ptr* values = saneData->array.create(sizeof(ptr), array->meta.count);
    if (!values) return NULL; // error: out of memory!

	SSDK_FORI(0, array->meta.max, 1)
		if (array->map[i].key)
            values[value++] = array->map[i].value;
	
    ((u32*)((byte*)values - ARRAY_HEAD_SIZE))[ARRAY_COUNT_FIELD] = array->meta.count;
    return values;
}
/* ---------------- HASH ARRAY ---------------- */


/* ---------------- API ---------------- */
byte ssdkInitDS(none) {
    if (saneData != NULL) return SSDK_TRUE;

    if (saneMemory == NULL) {
        if (saneLog != NULL) {
            saneLog->log(SANE_LOG_ERROR, "SaneMemory Not Initialized!");
        } else { printf("SaneMemory Not Initialized!\n"); }
        return SSDK_FALSE;
    }
    
    saneData = malloc(sizeof(SaneData));
    if (saneData == NULL) return SSDK_FALSE;  // error: out of memory!

    saneData->array.create = _createArrayImpl;
    saneData->array.pop = _popArrayImpl;
    saneData->array.put = _putArrayImpl;
    saneData->array.push = _pushArrayImpl;
    saneData->array.pull = _pullArrayImpl;
    saneData->array.resize = _resizeArrayImpl;
    saneData->array.destroy = _destroyArrayImpl;
    saneData->array.getHeader = _getHeaderImpl;

    saneData->linkedArray.create = _createLinkedArrayImpl;
    saneData->linkedArray.destroy = _destroyLinkedArrayImpl;
    saneData->linkedArray.collapse = _collapseLinkedArrayImpl;

    saneData->hashArray.create = _createHashArrayImpl;
    saneData->hashArray.destroy = _destroyHashArrayImpl;
    
    saneData->hashArray.put = _putHashArrayImpl;
    saneData->hashArray.pull = _pullHashArrayImpl;
    
    saneData->hashArray.get = _getHashArrayImpl;
    saneData->hashArray.getKeys = _getHashArrayKeysImpl;
    saneData->hashArray.getValues = _getHashArrayValuesImpl;
    
    saneData->module.mask = 0;
    saneData->module.calls = 0;
    saneData->module.name = "SaneData";
    saneData->module.size = sizeof(SaneData);
    return SSDK_TRUE;
}

none ssdkExitDS(none) {
    if (saneData == NULL) return;
	free(saneData);
    saneData = NULL;
}
/* ---------------- API ---------------- */
