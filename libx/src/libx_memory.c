#include "../include/libx_memory.h"

_libx_memory_api* memx = NULL;

/* ---------------- STANDARD ---------------- */
void _dealloc_impl(void* ptr) {
	if (!ptr) return;	// error: null ptr!
	// retrieve the pointer difference stored before the ptr
	u16 diff = *((u16*)ptr - 1);

	// retrieve+free the original pointer by shifting the aligned pointer
	free((void*)((u64)ptr - diff));
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
	if (!ptr) return memx->alloc(size, align);	// error: null ptr!
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


/* ---------------- LINEAR ALLOCATOR ---------------- */
Linear_Allocator* _create_linear_allocator_impl(u64 max, u64 align) {
	if (!max || !LIBX_IP2(align)) return NULL;	// error: value error!
	
	Linear_Allocator* allocator = (Linear_Allocator*)memx->alloc(sizeof(Linear_Allocator), align);
	if (!allocator) return NULL;	// error: out of memory!

	allocator->data = memx->alloc(max, align);
	if (!allocator->data) return NULL;

	allocator->offset = 0;
	allocator->max = max;
	return allocator;
}

void _linear_reset_impl(Linear_Allocator* allocator) {
	if (!allocator) return;	// error: null ptr!
	allocator->offset = 0;
}

void* _linear_alloc_impl(Linear_Allocator* allocator, u64 size, u64 align) {
	if (!allocator || !allocator->data) return NULL;	// error: null ptr!

	u64 aoffset = LIBX_ALIGN(allocator->offset, align);
	if (aoffset + size > allocator->max) return NULL;	// error: out of memory!

	allocator->offset = aoffset + size;
	return (u8*)allocator->data + aoffset;
}

void _destroy_linear_allocator_impl(Linear_Allocator* allocator) {
	if (!allocator || !allocator->data) return;	// error: null ptr!
	memx->dealloc(allocator->data);
	allocator->data = NULL;
	allocator->offset = 0;
	allocator->max = 0;
}
/* ---------------- LINEAR ALLOCATOR ---------------- */


/* ---------------- ARENA ALLOCATOR ---------------- */
Arena_Allocator* _create_arena_allocator_impl(Arena_Allocator* allocator, u64 max, u64 align) {
    if (!max || !LIBX_IP2(align)) return NULL; // error: invalid size

    Arena_Allocator* new_arena = (Arena_Allocator*)memx->alloc(sizeof(Arena_Allocator), align);
    if (!new_arena) return NULL; // error: out of memory

    new_arena->data = memx->alloc(max, align);
    if (!new_arena->data) {
        memx->dealloc(new_arena);
        return NULL;	// error: out of memory!
    }

    new_arena->offset = 0;
    new_arena->max = max;
    new_arena->next = NULL;
    new_arena->last = NULL;

    if (allocator) {
        new_arena->last = allocator;
        if (allocator->next) {
            new_arena->next = allocator->next;
            allocator->next->last = new_arena;
        }
        allocator->next = new_arena;
    }

    return new_arena;
}

void _arena_reset_impl(Arena_Allocator* allocator) {
	if (!allocator) return;	// error: null ptr!
	allocator->offset = 0;
}

void* _arena_alloc_impl(Arena_Allocator* allocator, u64 size, u64 align) {
    if (!allocator) return NULL; // error: invalid allocator

	// search for a node with enough space
    Arena_Allocator* node = allocator;
    while (node) {
        u64 aoffset = LIBX_ALIGN(node->offset, align);
        if (aoffset + size <= node->max) {
            void* ptr = (u8*)node->data + aoffset;
            node->offset = aoffset + size;
            return ptr;
        }
        node = node->next;
    }

    // no space, allocate a new node and store it in the linked list
    Arena_Allocator* new_node = memx->create_arena_allocator(allocator, allocator->max + size, align);
    if (!new_node) return NULL;

    new_node->offset = size;
    return new_node->data;
}

void _destroy_arena_allocator_impl(Arena_Allocator* allocator) {
    if (!allocator) return;
	
    if (allocator->next) allocator->next->last = allocator->last;
    if (allocator->last) allocator->last->next = allocator->next;
	
    memx->dealloc(allocator->data);
    memx->dealloc(allocator);

	allocator->data = NULL;
	allocator->last = NULL;
	allocator->next = NULL;
}

void _collapse_arena_allocator_impl(Arena_Allocator* allocator) {
    if (!allocator) return;

    Arena_Allocator* next = allocator->next;
    while (next) {
        Arena_Allocator* temp = next->next;
        memx->destroy_arena_allocator(next);
        next = temp;
    }

	Arena_Allocator* last = allocator->last;
    while (last) {
        Arena_Allocator* temp = last->last;
        memx->destroy_arena_allocator(last);
        last = temp;
    }
    
	memx->destroy_arena_allocator(allocator);
}
/* ---------------- ARENA ALLOCATOR ---------------- */


/* ---------------- STACK ALLOCATOR ---------------- */
/* ---------------- STACK ALLOCATOR ---------------- */


/* ---------------- RING ALLOCATOR ---------------- */
/* ---------------- RING ALLOCATOR ---------------- */



/* ---------------- API ---------------- */
u8 libx_init_memory(void) {
    if (memx != NULL) return LIBX_TRUE;    // redundant call: memory API already initialized!

	memx = (_libx_memory_api*)malloc(sizeof(_libx_memory_api));
	if (!memx) return LIBX_FALSE;

	memx->alloc = _alloc_impl;
	memx->dealloc = _dealloc_impl;
	memx->realloc = _realloc_impl;
	
	memx->create_linear_allocator = _create_linear_allocator_impl;
	memx->linear_alloc = _linear_alloc_impl;
	memx->linear_reset = _linear_reset_impl;
	memx->destroy_linear_allocator = _destroy_linear_allocator_impl;
	
	memx->create_arena_allocator = _create_arena_allocator_impl;
	memx->arena_alloc = _arena_alloc_impl;
	memx->arena_reset = _arena_reset_impl;
	memx->destroy_arena_allocator = _destroy_arena_allocator_impl;
	memx->collapse_arena_allocator = _collapse_arena_allocator_impl;

	return LIBX_TRUE;
}

void libx_cleanup_memory(void) {
    if (memx == NULL) return;    // error: memory API not initialized!
	free(memx);
	memx = NULL;
}
/* ---------------- API ---------------- */
