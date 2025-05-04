#include <corex/corex.h>

byte _create_string_impl(u32 size, String* string) {
    if (!string || !size || size >= COREX_STR_MAX) return COREX_FALSE;
    
    string->buffer = (byte*)corex->dsx.array.create_array(sizeof(byte), size+1);

    if (!string->buffer) return COREX_FALSE;

    string->meta = corex->dsx.array.get_array_head(string->buffer);
    string->buffer[size] = '\0';
    return COREX_TRUE;
}

byte _destroy_string_impl(String* string) {
    if (!string || !string->buffer) return COREX_FALSE;
    corex->dsx.array.destroy_array(string->buffer);
    string->buffer = NULL;
    string->meta.stride = 0;
    string->meta.count = 0;
    string->meta.size = 0;
    string->meta.max = 0;
    return COREX_TRUE;
}

u32 _len_string_impl(String* string) {
    if (!string || !string->buffer) return COREX_FALSE;
    return string->meta.count;
}

byte _copy_string_impl(String* src, String* dest) {
    if (!src || !src->buffer || !dest || !dest->buffer) return COREX_FALSE;
    return corex->strx.grow(src->meta.count, src->buffer, dest);
}

byte _ncopy_string_impl(u32 size, String* src, String* dest) {
    if (!src || !src->buffer || !dest || !dest->buffer) return COREX_FALSE;
    return corex->strx.grow(size, src->buffer, dest);
}

byte _grow_string_impl(u32 count, byte* chars, String* string) {
    if (!count || count >= COREX_STR_MAX || !chars || !string || !string->buffer) {
        return COREX_FALSE;
    }
    
    Array_Head head = corex->dsx.array.get_array_head(string->buffer);
    COREX_FORI(head.count, count, 1) {
        corex->dsx.array.put_array(string->buffer, i, chars + i);
    }
    
    string->meta = corex->dsx.array.get_array_head(string->buffer);
    string->buffer[count] = '\0';
    return COREX_TRUE;
}


byte _corex_init_strx(void) {
    if (!corex) return COREX_FALSE; // error: null ptr!
    if (corex->strx.init == COREX_TRUE) return COREX_TRUE;    // redundant call: Genx API already initialized!

    if (corex->memx.init == COREX_FALSE) {
        printf("Corex Memx API not initialized!\n");
        return COREX_FALSE; // error: failed to initialize Memx API!
    }
    
    if (corex->dsx.init == COREX_FALSE) {
        printf("Corex Dsx API not initialized!\n");
        return COREX_FALSE; // error: failed to initialize Dsx API!
    }

    corex->strx.create = _create_string_impl;
    corex->strx.destroy = _destroy_string_impl;
    corex->strx.len = _len_string_impl;
    corex->strx.copy = _copy_string_impl;
    corex->strx.ncopy = _ncopy_string_impl;
    corex->strx.grow = _grow_string_impl;

    return COREX_TRUE;
}

void _corex_cleanup_strx(void) {
    if (corex->strx.init == COREX_FALSE) return;    // error: Genx API not initialized!
	corex->meta.usage.apis &= ~COREX_GENX;
	corex->strx.init = COREX_FALSE;
    corex->meta.usage.strx = 0;
	corex->strx	= (Strx){0};
}
