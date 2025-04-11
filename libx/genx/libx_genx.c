#include <libx/libx.h>

#define BUFFER_VALID(b) ((b) && (b)->data && (b)->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_VALID)
#define BUFFER_CAN_WRITE(b) ((b)->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_RW)
#define BUFFER_CAN_READ(b) ((b)->desc[BUFFER_FIELD_ACCESS] != BUFFER_ACCESS_NONE)

u8 _depth8_impl(u8 value) {
u8 depth = 0;
    LIBX_FOR(u8, bit, 0, 8, 1) {
        depth = (LIBX_GET_BITS(value, bit)) ? bit+1 : depth;
    }
    return depth;
}

u8 _pack8_impl(u8 value1, u8 value2) {
    u8 depth = libx->genx.bin.depth8(value2);
    u8 packed = 0;
    packed = value1 << depth;
    packed |= value2;
    return packed;
}

u8* _unpack8_impl(u8 value, u8 depth, u8* out) {
    u8 lmask = (1 << depth) - 1;
    out[0] = value >> depth;
    out[1] = value & lmask;
    return out;
}


u8 _create_buffer_impl(Buffer_Layout layout, Buffer* buffer) {
    if (!buffer ||
        !layout.size || layout.size > BUFFER_SIZE_MAX   ||
        !layout.type || layout.type > BUFFER_TYPES      ||
        !layout.access || layout.access > BUFFER_ACCESS_TYPES) {
        return LIBX_FALSE;  // error: null ptr/value error!
    }

    buffer->data = libx->memx.alloc(layout.size, 8);
    if (!buffer->data) return LIBX_FALSE;   // error: out of memory!

    buffer->desc[BUFFER_FIELD_TYPE] = layout.type;
    buffer->desc[BUFFER_FIELD_SIZE] = layout.size;
    buffer->desc[BUFFER_FIELD_ACCESS] = layout.access;
    buffer->desc[BUFFER_FIELD_STATE] = BUFFER_STATE_VALID;
    
    buffer->desc[BUFFER_FIELD_WRITTEN] = 0;
    buffer->desc[BUFFER_FIELD_BIT] = 0;
    buffer->desc[BUFFER_FIELD_BYTE] = 0;
    buffer->desc[BUFFER_FIELD_FLAGS] = 0;

    return LIBX_TRUE;
}

u8 _read_buffer_impl(u64 count, void* bytes, Buffer* buffer) {
    if (!buffer || !bytes || !count ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_NONE)   ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_ALLOC)    ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_FREE)     ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_INVALID)) {
        return LIBX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }

    if (LIBX_GET_BITS(buffer->desc[BUFFER_FIELD_FLAGS], BUFFER_FLAG_AUTOCURSOR)) buffer->desc[BUFFER_FIELD_BYTE] = 0;
    LIBX_FOR(u64, byte, 0, count, 1) {
        if (!((u8*)buffer->data + byte)) break;    // error: null byte encountered!
        *((u8*)bytes + byte) = *(((u8*)buffer->data + buffer->desc[BUFFER_FIELD_BYTE]) + byte);
    }
    
    return LIBX_TRUE;
}

u8 _write_buffer_impl(u64 count, void* bytes, Buffer* buffer) {
    if (!buffer || !bytes || !count ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_R)       ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_NONE)    ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_ALLOC)     ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_FREE)      ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_INVALID)) {
        return LIBX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }

    if (count > buffer->desc[BUFFER_FIELD_SIZE] - buffer->desc[BUFFER_FIELD_WRITTEN])
        return LIBX_FALSE;  // error: not enough space for this write!

    LIBX_FOR(u64, byte, 0, count, 1) {
        if (!((u8*)bytes + byte)) break;    // error: null byte encountered!
        *(((u8*)buffer->data + buffer->desc[BUFFER_FIELD_BYTE]) + byte) = *((u8*)bytes + byte);
    }
    
    buffer->desc[BUFFER_FIELD_BYTE] += (LIBX_GET_BITS(buffer->desc[BUFFER_FIELD_FLAGS], BUFFER_FLAG_AUTOCURSOR)) ? count : 0;
    buffer->desc[BUFFER_FIELD_WRITTEN]+=count;
    return LIBX_TRUE;
}

u8 _bit_cursor_impl(u8 bit, Buffer* buffer) {
    if (!buffer || !bit || (bit >= 8) ||
        (LIBX_GET_BITS(buffer->desc[BUFFER_FIELD_FLAGS], BUFFER_FLAG_AUTOCURSOR))   ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_R)                      ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_NONE)                   ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_ALLOC)                    ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_FREE)                     ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_INVALID)) {
        return LIBX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }
    buffer->desc[BUFFER_FIELD_BIT] = bit;
    return LIBX_TRUE;
}

u8 _byte_cursor_impl(u64 byte, Buffer* buffer) {
    if (!buffer || !byte || (byte >= buffer->desc[BUFFER_FIELD_SIZE])               ||
        (LIBX_GET_BITS(buffer->desc[BUFFER_FIELD_FLAGS], BUFFER_FLAG_AUTOCURSOR))   ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_R)                      ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_NONE)                   ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_ALLOC)                    ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_FREE)                     ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_INVALID)) {
        return LIBX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }
    buffer->desc[BUFFER_FIELD_BYTE] = byte;
    return LIBX_TRUE;
}

u8 _set_flag_impl(Buffer_Flag flag, Buffer* buffer) {
    if (!buffer ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_R)       ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_NONE)    ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_INVALID)) {
        return LIBX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }
    buffer->desc[BUFFER_FIELD_FLAGS] = LIBX_SET_BITS(buffer->desc[BUFFER_FIELD_FLAGS], flag);
    return LIBX_TRUE;
}

u8 _has_flag_impl(Buffer_Flag flag, Buffer* buffer) {
    if (!buffer ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_R)       ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_NONE)    ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_INVALID)) {
        return LIBX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }
    return (LIBX_GET_BITS(buffer->desc[BUFFER_FIELD_FLAGS], flag)) ? LIBX_TRUE : LIBX_FALSE;
}

u8 _rem_flag_impl(Buffer_Flag flag, Buffer* buffer) {
    if (!buffer ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_R)       ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_NONE)    ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_INVALID)) {
        return LIBX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }
    buffer->desc[BUFFER_FIELD_FLAGS] = LIBX_REM_BITS(buffer->desc[BUFFER_FIELD_FLAGS], flag);
    return LIBX_TRUE;
}

u8 _set_field_impl(u8 field, u8 value, Buffer* buffer) {
    if (!buffer || field < 0 || field >= BUFFER_FIELDS ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_R)       ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_NONE)    ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_ALLOC)     ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_FREE)      ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_INVALID)) {
        return LIBX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }
    buffer->desc[field] = value;
    return LIBX_TRUE;
}

u8 _get_field_impl(u8 field, Buffer* buffer) {
    if (!buffer || field < 0 || field >= BUFFER_FIELDS ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_R)       ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_NONE)    ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_ALLOC)     ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_FREE)      ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_INVALID)) {
        return LIBX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }
    return buffer->desc[field];
}

u8 _reset_impl(Buffer* buffer) {
    if (!buffer ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_R)      ||
        (buffer->desc[BUFFER_FIELD_ACCESS] == BUFFER_ACCESS_NONE)   ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_ALLOC)    ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_FREE)     ||
        (buffer->desc[BUFFER_FIELD_STATE] == BUFFER_STATE_INVALID)) {
        return LIBX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }
    
    libx->memx.zero(buffer->data, buffer->desc[BUFFER_FIELD_SIZE]);
    buffer->desc[BUFFER_FIELD_WRITTEN] = 0;
    buffer->desc[BUFFER_FIELD_BYTE] = 0;
    buffer->desc[BUFFER_FIELD_BIT] = 0;
    return LIBX_TRUE;
}

u8 _destroy_buffer_impl(Buffer* buffer) {
    if (!buffer) return LIBX_FALSE;  // error: null ptr!

    libx->memx.dealloc(buffer->data);

    buffer->desc[BUFFER_FIELD_TYPE] = BUFFER_TYPE_NONE;
    buffer->desc[BUFFER_FIELD_SIZE] = BUFFER_SIZE_NONE;
    buffer->desc[BUFFER_FIELD_ACCESS] = BUFFER_ACCESS_NONE;
    buffer->desc[BUFFER_FIELD_STATE] = BUFFER_STATE_NONE;
    
    buffer->desc[BUFFER_FIELD_WRITTEN] = 0;
    buffer->desc[BUFFER_FIELD_BIT] = 0;
    buffer->desc[BUFFER_FIELD_BYTE] = 0;
    buffer->desc[BUFFER_FIELD_FLAGS] = 0;

    return LIBX_TRUE;
}


u8 _libx_init_genx(void) {
    if (!libx) return LIBX_FALSE; // error: null ptr!
    if (libx->genx.init == LIBX_TRUE) return LIBX_TRUE;    // redundant call: Genx API already initialized!

    if (libx->memx.init == LIBX_FALSE) {
        printf("Libx Memx API not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize Memx API!
    }
    
    if (libx->dsx.init == LIBX_FALSE) {
        printf("Libx Dsx API not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize Dsx API!
    }

    libx->genx.buffer.create = _create_buffer_impl;
    libx->genx.buffer.read = _read_buffer_impl;
    libx->genx.buffer.write = _write_buffer_impl;
    libx->genx.buffer.bit_cursor = _bit_cursor_impl;
    libx->genx.buffer.byte_cursor = _byte_cursor_impl;
    libx->genx.buffer.set_flag = _set_flag_impl;
    libx->genx.buffer.has_flag = _has_flag_impl;
    libx->genx.buffer.rem_flag = _rem_flag_impl;
    libx->genx.buffer.set_field = _set_field_impl;
    libx->genx.buffer.get_field = _get_field_impl;
    libx->genx.buffer.reset = _reset_impl;
    libx->genx.buffer.destroy = _destroy_buffer_impl;

    libx->genx.bin.depth8 = _depth8_impl;
    libx->genx.bin.pack8 = _pack8_impl;
    libx->genx.bin.unpack8 = _unpack8_impl;

	libx->genx.init = LIBX_TRUE;
    return LIBX_TRUE;
}

void _libx_cleanup_genx(void) {
    if (libx->genx.init == LIBX_FALSE) return;    // error: Genx API not initialized!
	libx->genx.init = LIBX_FALSE;
	libx->genx	= (Genx){0};
}
