#include <include/SSDK/SSDK.h>

SaneMemory* saneMemory = NULL;

/* ---------------- STANDARD ---------------- */
void _dealloc_impl(void* ptr) {
	saneMemory->module.calls++;
	if (!ptr) return;	// error: null ptr!
	// retrieve the pointer difference stored before the ptr
	u16 diff = *((u16*)ptr - 1);

	// retrieve+free the original pointer by shifting the aligned pointer
	free((void*)((u64)ptr - diff));
}

void _zero_impl(void* ptr, u64 size) {
	saneMemory->module.calls++;
	if (!ptr || !size) return;	// error: null ptr/value error!
	memset(ptr, 0, size);
}

void* _alloc_impl(u64 size, u64 align) {
	saneMemory->module.calls++;
	if (!size || !SSDK_IP2(align)) return NULL;	// error: value error!

	// allocate 2 extra bytes for pointer difference
	// allocate align-1 extra bytes for pointer alignment
	void* optr = malloc(sizeof(u16) + (align - 1) + size);
	if (!optr) return NULL;	// error: out of memory!

	// move 2 bytes from optr align, zero, and return that address
	void* aptr = memset((void*)SSDK_ALIGN((u64)((u16*)optr + 1), align), 0, size);

	// store pointer diff/offset at the head of the aligned pointer;
	*((u16*)aptr - 1) = (u16)((u64)aptr - (u64)optr);

	return aptr;
}

void* _realloc_impl(void* ptr, u64 size, u64 align) {
	saneMemory->module.calls++;
	if (!ptr) return saneMemory->alloc(size, align);	// error: null ptr!
	if (!size || !SSDK_IP2(align)) return NULL;	// error: value error!
	
	// retrieve the pointer difference stored before the ptr
	u16 diff = *((u16*)ptr - 1);
	
	// retrieve the original pointer by shifting the aligned pointer
	void* optr = (void*)((u64)ptr - diff);

	// allocate 2 extra bytes for pointer difference
	// allocate align-1 extra bytes for pointer alignment
	void* nptr = realloc((void*)optr, sizeof(u16) + (align - 1) + size);
	if (!nptr) return NULL;

	// move 2 bytes from optr align, and assign that address
	void* aptr = (void*)SSDK_ALIGN((u64)((u16*)nptr + 1), align);

	// store pointer diff/offset at the head of the aligned pointer;
	*((u16*)aptr - 1) = (u16)((u64)aptr - (u64)nptr);

	return aptr;
}
/* ---------------- STANDARD ---------------- */


/* ---------------- ALLOCATOR ---------------- */
byte _create_allocator_impl(SaneAllocatorType type, u64 max, u64 align, SaneAllocator* allocator) {
	saneMemory->module.calls++;
	if (type >= SANE_ALLOC_TYPES || !max || !SSDK_IP2(align) || !allocator) return SSDK_FALSE;	// error value error!
	switch (type) {
		case SANE_ALLOC_DEFAULT:
		case SANE_ALLOC_ARENA: {
			allocator->type = type;
	
			allocator->context.arena.buffer = saneMemory->alloc(max, align);
			if (!allocator->context.arena.buffer) return SSDK_FALSE;
		
			allocator->context.arena.offset = 0;
			allocator->context.arena.max = max;
			return SSDK_TRUE;
		}
		default: break;
	}
	return SSDK_FALSE;
}

byte _destroy_allocator_impl(SaneAllocator* allocator) {
	saneMemory->module.calls++;
	if (!allocator || allocator->type >= SANE_ALLOC_TYPES) return SSDK_FALSE;	// error: value error!
	switch (allocator->type)
	{
	case SANE_ALLOC_DEFAULT:
	case SANE_ALLOC_ARENA: {
		if (!allocator->context.arena.buffer) return SSDK_FALSE;	// error: null ptr!
		saneMemory->dealloc(allocator->context.arena.buffer);
		allocator->context.arena.buffer = NULL;
		allocator->context.arena.offset = 0;
		allocator->context.arena.max = 0;
		return SSDK_TRUE;
	}
	default: break;
	}
	return SSDK_TRUE;
}
/* ---------------- ALLOCATOR ---------------- */


/* ---------------- ARENA ALLOCATOR ---------------- */
void _arena_reset_impl(SaneAllocator* allocator) {
	saneMemory->module.calls++;
	if (!allocator) return;	// error: null ptr!
	allocator->context.arena.offset = 0;
}

void* _arena_alloc_impl(u64 size, u64 align, SaneAllocator* allocator) {
	saneMemory->module.calls++;
	if (!allocator || !allocator->context.arena.buffer) return NULL;	// error: null ptr!

	u64 aoffset = SSDK_ALIGN(allocator->context.arena.offset, align);
	if (aoffset + size > allocator->context.arena.max) return NULL;	// error: out of memory!

	allocator->context.arena.commit += size;
	allocator->context.arena.offset = aoffset + size;
	return (byte*)allocator->context.arena.buffer + aoffset;
}
/* ---------------- ARENA ALLOCATOR ---------------- */


/* -------------------- GENERICS ------------------ */
byte _blob_alloc_impl(u64 align, SaneBlob* blob) {
	saneMemory->module.calls++;
	if (!blob || !SSDK_IP2(align) || !blob->size) {
		blob->size = 0;
		return SSDK_FALSE;	// error: null ptr/value error!
	}
	
	blob->data = saneMemory->alloc(blob->size, align);
	if (!blob->data) {
		blob->size = 0;
		return SSDK_FALSE;	// error: out of memory!
	}

	return SSDK_TRUE;
}

byte _blob_realloc_impl(u64 size, u64 align, SaneBlob* blob) {
	saneMemory->module.calls++;
	if (!blob || !blob->data || !size || size > INT_MAX || !SSDK_IP2(align)) return SSDK_FALSE;	// error: null ptr/value error!

	void* temp = saneMemory->realloc(blob->data, size, align);
	if (!temp) return SSDK_FALSE;	// error: out of memory!

	blob->data = temp;
	blob->size = size;

	return SSDK_TRUE;
}

byte _blob_dealloc_impl(SaneBlob* blob) {
	saneMemory->module.calls++;
	if (!blob || !blob->data || !blob->size) return SSDK_FALSE;	// error: null ptr/value error!
	saneMemory->dealloc(blob->data);
	blob->data = (void*)0;
	blob->size = 0;
	return SSDK_TRUE;
}
/* -------------------- GENERICS ------------------ */


/* ---------------- API ---------------- */
byte ssdkInitMemory(none) {
    if (saneMemory != NULL) return SSDK_TRUE;

	saneMemory = malloc(sizeof(SaneMemory));
	if (!saneMemory) return SSDK_FALSE;	// error out of memory!

	saneMemory->alloc = _alloc_impl;
	saneMemory->zero = _zero_impl;
	saneMemory->dealloc = _dealloc_impl;
	saneMemory->realloc = _realloc_impl;

	saneMemory->blob_alloc = _blob_alloc_impl;
	saneMemory->blob_realloc = _blob_realloc_impl;
	saneMemory->blob_dealloc = _blob_dealloc_impl;
	
	saneMemory->create_allocator = _create_allocator_impl;
	saneMemory->arena_alloc = _arena_alloc_impl;
	saneMemory->arena_reset = _arena_reset_impl;
	saneMemory->destroy_allocator = _destroy_allocator_impl;
	
	saneMemory->module.mask = 0;
    saneMemory->module.calls = 0;
    saneMemory->module.name = "SaneMemory";
    saneMemory->module.size = sizeof(SaneMemory);
	return SSDK_TRUE;
}

void ssdkExitMemory(none) {
    if (saneMemory == NULL) return;    // error: memx API not initialized!
	free(saneMemory);
	saneMemory = NULL;
}
/* ---------------- API ---------------- */
