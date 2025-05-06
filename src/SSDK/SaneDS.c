#include <include/SSDK/SSDK.h>

SaneDS* saneDS = NULL;

/* ---------------- ARRAY ---------------- */
#define ARRAY_HEAD_SIZE (sizeof(u32) * 4)

ptr _createArrayImpl_(u32 stride, u32 max) {
    u32* head = (u32*)saneMemory->alloc(ARRAY_HEAD_SIZE + (stride * max), 8);
    if (!head) return NULL; // error: out of memory!

    head[ARRAY_SIZE_FIELD] = ARRAY_HEAD_SIZE + (stride * max);
    head[ARRAY_STRIDE_FIELD] = stride;
    head[ARRAY_COUNT_FIELD] = 0;
    head[ARRAY_MAX_FIELD] = max;

    return (ptr)((byte*)head + ARRAY_HEAD_SIZE);
}

none _putArrayImpl_(ptr array, u32 index, ptr invalue) {
    if (!array || !invalue || index < 0) return;
	
    u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    if (index >= head[ARRAY_MAX_FIELD] || (head[ARRAY_COUNT_FIELD] + 1) > head[ARRAY_MAX_FIELD]) return;

    u32 offset = (head[ARRAY_STRIDE_FIELD] * index);

    ptr slot = ((byte*)array + offset);
    memcpy(slot, invalue, head[ARRAY_STRIDE_FIELD]);

    head[ARRAY_COUNT_FIELD]++;
}

none _pullArrayImpl_(ptr array, u32 index, ptr outvalue) {
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

none _pushArrayImpl_(ptr array, ptr invalue) {
    if (!array || !invalue) return;
	
	u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    u32 index = head[ARRAY_COUNT_FIELD];

    saneDS->array.put(array, index, invalue);
}

none _popArrayImpl_(ptr array, ptr outvalue) {
    if (!array || outvalue == NULL) return;
	
	u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    if (!head[ARRAY_COUNT_FIELD]) return;

    u32 index = (head[ARRAY_COUNT_FIELD] - 1);

    saneDS->array.pull(array, index, outvalue);
}

ptr _resizeArrayImpl_(ptr array, u32 max) {
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

none _destroyArrayImpl_(ptr array) {
    saneMemory->dealloc((ptr)((byte*)array - ARRAY_HEAD_SIZE));
}

SaneArrayHeader _getHeaderImpl_(ptr array) {
    SaneArrayHeader arrhead = {0, 0, 0, 0};
    if (!array) return arrhead; // error: null ptr!

    u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    
    arrhead.size = head[ARRAY_SIZE_FIELD];
    arrhead.stride = head[ARRAY_STRIDE_FIELD];
    arrhead.count = head[ARRAY_COUNT_FIELD];
    arrhead.max = head[ARRAY_MAX_FIELD];

    return arrhead;
}

none _setArrayHeadImpl_(ptr array, u32 field, u32 value) {
    if (!array || field >= ARRAY_FIELDS) return;
    u32* head = (u32*)((byte*)array - ARRAY_HEAD_SIZE);
    head[field] = value;
}
/* ---------------- ARRAY ---------------- */


/* ---------------- LINKED ARRAY ---------------- */
none _createLinkedArrayImpl_(u32 stride, u32 max, SaneLinkedArray* array) {
    if (!array) return;  // error: null ptr!
    if (!stride || !max) return;   // error: value error!

    if (array->array != NULL) {
        array->next = saneMemory->alloc(sizeof(SaneLinkedArray), 8);
        if (!array->next) return;  // error: out of memory!
    
        array->next->array = saneDS->array.create(stride, max);
        if (!array->next->array) {
            saneMemory->dealloc(array->next);
            return;    // error: out of memory!
        }

        array->next->next = NULL;
        array->next->last = array;
        array->next->meta = saneDS->array.getHeader(array->next->array);
    } else {
        array->array = saneDS->array.create(stride, max);
        if (!array->array) return;    // error: out of memory!
        array->next = NULL;
        array->last = NULL;
        array->meta = saneDS->array.getHeader(array->array);
    }
}

none _destroyLinkedArrayImpl_(SaneLinkedArray* array) {
    if (!array) return; // error: null ptr!
    
    if (array->next) array->next->last = array->last;
    if (array->last) array->last->next = array->next;
    
    saneDS->array.destroy(array->array);
    saneMemory->dealloc(array);
    
    array->meta = (SaneArrayHeader){0};
    array->array = NULL;
    array->last = NULL;
    array->next = NULL;
}

none _collapseLinkedArrayImpl_(SaneLinkedArray* array) {
    if (!array) return; // error: null ptr!
    
    SaneLinkedArray* next = array->next;
    while (next) {
        SaneLinkedArray* temp = next->next;
        saneDS->array.destroy(next);
        next = temp;
    }

	SaneLinkedArray* last = array->last;
    while (last) {
        SaneLinkedArray* temp = last->last;
        saneDS->array.destroy(last);
        last = temp;
    }
    
	saneDS->array.destroy(array);
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

i32 _probeKeyHashArray_(cstr key, SaneHashArray* array) {
    if (!array || !key) return -1;

    SaneArrayHeader meta = saneDS->array.getHeader(array->map);

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

i32 _probeSlotHashArray_(cstr key, SaneHashArray* array) {
    if (!array || !key) return -1;
    
    SaneArrayHeader meta = saneDS->array.getHeader(array->map);
    
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

none _createHashArrayImpl_(u32 max, SaneHashArray* array) {
    if (!array) return;  // error: out of memory!

    array->map = (SaneKeyValue*)saneDS->array.create(sizeof(SaneKeyValue), max);
    if (!array->map) {
        saneMemory->dealloc(array);
        return; // error: out of memory!
    }

    SSDK_FORI(0, max, 1) {
        array->map[i] = (SaneKeyValue){
            .value = NULL,
            .key = NULL
        };
    }

    array->meta = saneDS->array.getHeader(array->map);
}

byte _putHashArrayImpl_(cstr key, ptr value, SaneHashArray* array) {
    if (!array || !key || !value) return SSDK_FALSE;  // error: null ptr!

    SaneArrayHeader meta = saneDS->array.getHeader(array->map);
    if (meta.count > (u32)(meta.max * 0.7)) {
        SaneKeyValue* temp = (SaneKeyValue*)saneDS->array.create(sizeof(SaneKeyValue), meta.max * 2);
        if (!temp) return SSDK_FALSE;  // error: out of memory!

        ((u32*)((byte*)temp - ARRAY_HEAD_SIZE))[ARRAY_COUNT_FIELD] = meta.count;
        memcpy(temp, array->map, meta.stride * meta.max);
        
        saneDS->array.destroy(array->map);
        array->map = temp;
    }
    
    i32 index = _probeSlotHashArray_(key, array);
    if (index == -1) return SSDK_FALSE;

    if (array->map[index].key && strcmp(array->map[index].key, key) == 0) {
        array->map[index].value = value;
    } else {
        saneDS->array.put(array->map, index, &(SaneKeyValue){ .value = value, .key = strdup(key)});
    }
    
    array->meta = saneDS->array.getHeader(array->map);
    return SSDK_TRUE;
}

ptr _getHashArrayImpl_(cstr key, SaneHashArray* array) {
    if (!array || !key) return NULL;  // error: null ptr!
    
    i32 index = _probeKeyHashArray_(key, array);
    if (index == -1) return NULL;

    return array->map[index].value;
}

byte _pullHashArrayImpl_(cstr key, SaneKeyValue* out, SaneHashArray* array) {
    if (!array || !key) return SSDK_FALSE;  // error: null ptr!

    i32 index = _probeKeyHashArray_(key, array);
    if (index == -1) return SSDK_FALSE;

    saneDS->array.pull(array->map, index, out);
    array->meta = saneDS->array.getHeader(array->map);
    return SSDK_TRUE;
}

none _destroyHashArrayImpl_(SaneHashArray* array) {
    if (!array) return;   // error: null ptr!
    saneDS->array.destroy(array->map);
    saneMemory->dealloc(array);
}

cstr* _getHashArrayKeysImpl_(SaneHashArray* array) {
    u8 key = 0;
	
    cstr* keys = saneDS->array.create(sizeof(cstr), array->meta.count);
    if (!keys) return NULL; // error: out of memory!

	SSDK_FORI(0, array->meta.max, 1)
		if (array->map[i].key)
            keys[key++] = array->map[i].key;
	
    ((u32*)((byte*)keys - ARRAY_HEAD_SIZE))[ARRAY_COUNT_FIELD] = array->meta.count;
    return keys;
}

ptr* _getHashArrayValuesImpl_(SaneHashArray* array) {
    u8 value = 0;
	
    ptr* values = saneDS->array.create(sizeof(ptr), array->meta.count);
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
    if (saneDS != NULL) return SSDK_TRUE;

    if (saneMemory == NULL) {
        if (saneLog != NULL) {
            saneLog->log(SANE_LOG_ERROR, "SaneMemory Not Initialized!");
        } else { printf("SaneMemory Not Initialized!\n"); }
        return SSDK_FALSE;
    }
    
    saneDS = malloc(sizeof(SaneDS));
    if (saneDS == NULL) return SSDK_FALSE;  // error: out of memory!

    saneDS->array.create = _createArrayImpl_;
    saneDS->array.pop = _popArrayImpl_;
    saneDS->array.put = _putArrayImpl_;
    saneDS->array.push = _pushArrayImpl_;
    saneDS->array.pull = _pullArrayImpl_;
    saneDS->array.resize = _resizeArrayImpl_;
    saneDS->array.destroy = _destroyArrayImpl_;
    saneDS->array.getHeader = _getHeaderImpl_;

    saneDS->linkedArray.create = _createLinkedArrayImpl_;
    saneDS->linkedArray.destroy = _destroyLinkedArrayImpl_;
    saneDS->linkedArray.collapse = _collapseLinkedArrayImpl_;

    saneDS->hashArray.create = _createHashArrayImpl_;
    saneDS->hashArray.destroy = _destroyHashArrayImpl_;
    
    saneDS->hashArray.put = _putHashArrayImpl_;
    saneDS->hashArray.pull = _pullHashArrayImpl_;
    
    saneDS->hashArray.get = _getHashArrayImpl_;
    saneDS->hashArray.getKeys = _getHashArrayKeysImpl_;
    saneDS->hashArray.getValues = _getHashArrayValuesImpl_;
    
    saneDS->module.mask = 0;
    saneDS->module.calls = 0;
    saneDS->module.name = "SaneDS";
    saneDS->module.size = sizeof(SaneDS);
    return SSDK_TRUE;
}

none ssdkExitDS(none) {
    if (saneDS == NULL) return;
	free(saneDS);
    saneDS = NULL;
}
/* ---------------- API ---------------- */
