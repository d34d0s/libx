#include <libx/libx.h>

/* ---------------- STANDARD ---------------- */
void _dealloc_impl(void* ptr) {
	if (!ptr) return;	// error: null ptr!
	// retrieve the pointer difference stored before the ptr
	u16 diff = *((u16*)ptr - 1);

	// retrieve+free the original pointer by shifting the aligned pointer
	free((void*)((u64)ptr - diff));
}

void _zero_impl(void* ptr, u64 size) {
	if (!ptr || !size) return;	// error: null ptr/value error!
	memset(ptr, 0, size);
}

void* _alloc_impl(u64 size, u64 align) {
	if (!size || !LIBX_IP2(align)) return NULL;	// error: value error!

	// allocate 2 extra bytes for pointer difference
	// allocate align-1 extra bytes for pointer alignment
	void* optr = malloc(sizeof(u16) + (align - 1) + size);
	if (!optr) return NULL;	// error: out of memory!

	// move 2 bytes from optr align, zero, and return that address
	void* aptr = memset((void*)LIBX_ALIGN((u64)((u16*)optr + 1), align), 0, size);

	// store pointer diff/offset at the head of the aligned pointer;
	*((u16*)aptr - 1) = (u16)((u64)aptr - (u64)optr);

	return aptr;
}

void* _realloc_impl(void* ptr, u64 size, u64 align) {
	if (!ptr) return libx->memx.alloc(size, align);	// error: null ptr!
	if (!size || !LIBX_IP2(align)) return NULL;	// error: value error!
	
	// retrieve the pointer difference stored before the ptr
	u16 diff = *((u16*)ptr - 1);
	
	// retrieve the original pointer by shifting the aligned pointer
	void* optr = (void*)((u64)ptr - diff);

	// allocate 2 extra bytes for pointer difference
	// allocate align-1 extra bytes for pointer alignment
	void* nptr = realloc((void*)optr, sizeof(u16) + (align - 1) + size);
	if (!nptr) return NULL;

	// move 2 bytes from optr align, and assign that address
	void* aptr = (void*)LIBX_ALIGN((u64)((u16*)nptr + 1), align);

	// store pointer diff/offset at the head of the aligned pointer;
	*((u16*)aptr - 1) = (u16)((u64)aptr - (u64)nptr);

	return aptr;
}
/* ---------------- STANDARD ---------------- */


/* ---------------- ARENA ALLOCATOR ---------------- */
void _arena_reset_impl(Allocator* allocator) {
	if (!allocator) return;	// error: null ptr!
	allocator->context.arena.offset = 0;
}

void* _arena_alloc_impl(u64 size, u64 align, Allocator* allocator) {
	if (!allocator || !allocator->context.arena.buffer) return NULL;	// error: null ptr!

	u64 aoffset = LIBX_ALIGN(allocator->context.arena.offset, align);
	if (aoffset + size > allocator->context.arena.max) return NULL;	// error: out of memory!

	allocator->context.arena.commit += size;
	allocator->context.arena.offset = aoffset + size;
	return (u8*)allocator->context.arena.buffer + aoffset;
}
/* ---------------- ARENA ALLOCATOR ---------------- */

/* ---------------- ALLOCATOR ---------------- */
u8 _create_allocator_impl(Alloc_Type type, u64 max, u64 align, Allocator* allocator) {
	if (type >= ALLOC_TYPES || !max || !LIBX_IP2(align) || !allocator) return LIBX_FALSE;	// error value error!
	switch (type) {
		case ALLOC_DEFAULT:
		case ALLOC_ARENA: {
			allocator->type = type;
	
			allocator->context.arena.buffer = libx->memx.alloc(max, align);
			if (!allocator->context.arena.buffer) return LIBX_FALSE;
		
			allocator->context.arena.offset = 0;
			allocator->context.arena.max = max;
			return LIBX_TRUE;
		}
		default: break;
	}
}

u8 _destroy_allocator_impl(Allocator* allocator) {
	if (!allocator || allocator->type >= ALLOC_TYPES) return LIBX_FALSE;	// error: value error!
	switch (allocator->type)
	{
	case ALLOC_DEFAULT:
	case ALLOC_ARENA: {
		if (!allocator->context.arena.buffer) return LIBX_FALSE;	// error: null ptr!
		libx->memx.dealloc(allocator->context.arena.buffer);
		allocator->context.arena.buffer = NULL;
		allocator->context.arena.offset = 0;
		allocator->context.arena.max = 0;
		return LIBX_TRUE;
	}
	default: break;
	}
	return LIBX_TRUE;
}
/* ---------------- ALLOCATOR ---------------- */

/* -------------------- GENERICS ------------------ */
u8 _blob_alloc_impl(Blob* blob, u64 align) {
	if (!blob || !LIBX_IP2(align) || !blob->size) {
		blob->size = 0;
		return LIBX_FALSE;	// error: null ptr/value error!
	}
	
	blob->data = libx->memx.alloc(blob->size, align);
	if (!blob->data) {
		blob->size = 0;
		return LIBX_FALSE;	// error: out of memory!
	}

	return LIBX_TRUE;
}

u8 _blob_realloc_impl(Blob* blob, u64 size, u64 align) {
	if (!blob || !blob->data || !size || size > INT_MAX || !LIBX_IP2(align)) return LIBX_FALSE;	// error: null ptr/value error!

	void* temp = libx->memx.realloc(blob->data, size, align);
	if (!temp) return LIBX_FALSE;	// error: out of memory!

	blob->data = temp;
	blob->size = size;

	return LIBX_TRUE;
}

u8 _blob_dealloc_impl(Blob* blob) {
	if (!blob || !blob->data || !blob->size) return LIBX_FALSE;	// error: null ptr/value error!
	libx->memx.dealloc(blob->data);
	blob->data = (void*)0;
	blob->size = 0;
	return LIBX_TRUE;
}
/* -------------------- GENERICS ------------------ */

/* ---------------- API ---------------- */
u8 _libx_init_memx(void) {
    if (!libx) return LIBX_FALSE; // error: null ptr!
    if (libx->memx.init == LIBX_TRUE) return LIBX_TRUE;    // redundant call: memx API already initialized!

	libx->memx.alloc = _alloc_impl;
	libx->memx.zero = _zero_impl;
	libx->memx.dealloc = _dealloc_impl;
	libx->memx.realloc = _realloc_impl;
	
	libx->memx.blob_alloc = _blob_alloc_impl;
	libx->memx.blob_dealloc = _blob_dealloc_impl;
	libx->memx.blob_realloc = _blob_realloc_impl;
	
	libx->memx.create_allocator = _create_allocator_impl;
	libx->memx.arena_alloc = _arena_alloc_impl;
	libx->memx.arena_reset = _arena_reset_impl;
	libx->memx.destroy_allocator = _destroy_allocator_impl;
	
	libx->meta.usage.memx = sizeof(Memx);
	libx->memx.init = LIBX_TRUE;
	return LIBX_TRUE;
}

void _libx_cleanup_memx(void) {
    if (libx->memx.init == LIBX_FALSE) return;    // error: memx API not initialized!
	libx->meta.usage.apis &= ~LIBX_MEMX;
	libx->memx.init = LIBX_FALSE;
	libx->meta.usage.memx = 0;
	libx->memx	= (Memx){0};
}
/* ---------------- API ---------------- */
