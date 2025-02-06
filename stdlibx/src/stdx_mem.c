#include "../include/stdx_mem.h"

void stdx_free(void *ptr) {
	if (!ptr) return;	// error: null ptr!
	// retrieve the pointer difference stored before the ptr
	u16 diff = *((u8 *)ptr - sizeof(u16));

	// retrieve+free the original pointer by shifting the aligned pointer
	void* optr = (void *)((u64)ptr - diff);
	free(optr);
}

void * stdx_alloc(u64 size, u64 align) {
	if (!size || !IP2(align)) return NULL;	// error: value error!

	// allocate 2 extra bytes for pointer difference
	// allocate align-1 extra bytes for pointer alignment
	void *optr = malloc(sizeof(u16) + (align - 1) + size);
	if (!optr) return NULL;	// error: out of memory!

	// move 16 bytes from optr and align+return that address
	void *aptr = (void *)ALIGN((u64)((u8 *)optr + sizeof(u16)), align);
	
	// store pointer diff/offset at the head of the aligned pointer;
	*((u8 *)aptr - sizeof(u16)) = (u16)((u64)aptr - (u64)optr);

	return aptr;
}

void * stdx_realloc(void *ptr, u64 size, u64 align) {
	if (!ptr) return stdx_alloc(size, align);	// error: null ptr!
	if (!size || !IP2(align)) return NULL;	// error: value error!
	
	// retrieve the pointer difference stored before the ptr
	u64 diff = *((u8 *)ptr - sizeof(u16));
	
	// retrieve the original pointer by shifting the aligned pointer
	void *optr = (void *)((u64)ptr - diff);

	// TODO: why does this fail?
	// allocate 2 extra bytes for pointer difference
	// allocate align-1 extra bytes for pointer alignment
	void *nptr = realloc(optr, sizeof(u16) + (align - 1) + size);
	if (!nptr) return NULL;	

	// move 16 bytes from optr and align+return that address
	void* aptr = (void *)ALIGN((u64)((u8*)nptr + sizeof(16)), align);

	// store pointer diff/offset at the head of the aligned pointer;
	*((u8 *)aptr - sizeof(u16)) = (u16)((u64)aptr - (u64)nptr);

	return aptr;
}
