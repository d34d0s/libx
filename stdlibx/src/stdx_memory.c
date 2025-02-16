#include "../include/stdx_memory.h"

_stdx_memory_api* memory_api = NULL;

void _dealloc_impl(void* ptr) {
	if (!ptr) return;	// error: null ptr!
	// retrieve the pointer difference stored before the ptr
	u16 diff = *((u16*)ptr - 1);

	// retrieve+free the original pointer by shifting the aligned pointer
	free((void*)((u64)ptr - diff));
}

void* _alloc_impl(u64 size, u64 align) {
	if (!size || !IP2(align)) return NULL;	// error: value error!

	// allocate 2 extra bytes for pointer difference
	// allocate align-1 extra bytes for pointer alignment
	void* optr = malloc(sizeof(u16) + (align - 1) + size);
	if (!optr) return NULL;	// error: out of memory!

	// move 2 bytes from optr and align+return that address
	void* aptr = (void*)ALIGN((u64)((u16*)optr + 1), align);

	// store pointer diff/offset at the head of the aligned pointer;
	*((u16*)aptr - 1) = (u16)((u64)aptr - (u64)optr);

	return aptr;
}

void* _realloc_impl(void* ptr, u64 size, u64 align) {
	if (!ptr) return memory_api->alloc(size, align);	// error: null ptr!
	if (!size || !IP2(align)) return NULL;	// error: value error!
	
	// retrieve the pointer difference stored before the ptr
	u16 diff = *((u16*)ptr - 1);
	
	// retrieve the original pointer by shifting the aligned pointer
	void* optr = (void*)((u64)ptr - diff);

	// allocate 2 extra bytes for pointer difference
	// allocate align-1 extra bytes for pointer alignment
	void* nptr = realloc((void*)optr, sizeof(u16) + (align - 1) + size);
	if (!nptr) return NULL;

	// move 2 bytes from optr and align+return that address
	void* aptr = (void*)ALIGN((u64)((u16*)nptr + 1), align);

	// store pointer diff/offset at the head of the aligned pointer;
	*((u16*)aptr - 1) = (u16)((u64)aptr - (u64)nptr);

	return aptr;
}

u8 stdx_init_memory(void) {
	memory_api = (_stdx_memory_api*)malloc(sizeof(_stdx_memory_api));
	if (!memory_api) return STDX_FALSE;

	memory_api->alloc = _alloc_impl;
	memory_api->dealloc = _dealloc_impl;
	memory_api->realloc = _realloc_impl;

	return STDX_TRUE;
}

void stdx_cleanup_memory(void) {
	free(memory_api);
}

