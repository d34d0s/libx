#include <corex/corex.h>

#define BUFFER_VALID(b) ((b) && (b)->data && (b)->meta.state == BUFFER_STATE_VALID)
#define BUFFER_CAN_WRITE(b) ((b)->meta.access == BUFFER_ACCESS_RW)
#define BUFFER_CAN_READ(b) ((b)->meta.access != BUFFER_ACCESS_NONE)


byte _depth8_impl(byte value) {
byte depth = 0;
    COREX_FOR(byte, bit, 0, 8, 1) {
        depth = (COREX_GET_BITS(value, bit)) ? bit+1 : depth;
    }
    return depth;
}

byte _pack8_impl(byte value1, byte value2) {
    byte depth = corex->genx.bin.depth8(value2);
    byte packed = 0;
    packed = value1 << depth;
    packed |= value2;
    return packed;
}

byte* _unpack8_impl(byte value, byte depth, byte* out) {
    byte lmask = (1 << depth) - 1;
    out[0] = value >> depth;
    out[1] = value & lmask;
    return out;
}


byte _create_buffer_impl(Buffer_Layout layout, Buffer* buffer) {
    if (!buffer ||
        !layout.size || layout.size > BUFFER_SIZE_MAX   ||
        !layout.type || layout.type > BUFFER_TYPES      ||
        !layout.access || layout.access > BUFFER_ACCESS_TYPES) {
        return COREX_FALSE;  // error: null ptr/value error!
    }

    buffer->data = corex->memx.alloc(layout.size, 8);
    if (!buffer->data) return COREX_FALSE;   // error: out of memory!

    buffer->meta.type = layout.type;
    buffer->meta.size = layout.size;
    buffer->meta.access = layout.access;
    buffer->meta.state = BUFFER_STATE_VALID;
    
    buffer->meta.cursor.written = 0;
    buffer->meta.cursor.bit = 0;
    buffer->meta.cursor.byte = 0;
    buffer->meta.flags = 0;

    return COREX_TRUE;
}

byte _reset_impl(Buffer* buffer) {
    if (!BUFFER_VALID(buffer)) {
        return COREX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }
    
    corex->memx.zero(buffer->data, buffer->meta.size);
    buffer->meta.cursor.written = 0;
    buffer->meta.cursor.byte = 0;
    buffer->meta.cursor.bit = 0;
    return COREX_TRUE;
}

byte _destroy_buffer_impl(Buffer* buffer) {
    if (!BUFFER_VALID(buffer)) return COREX_FALSE;  // error: null ptr!

    corex->memx.dealloc(buffer->data);

    buffer->meta.state = BUFFER_STATE_FREE;
    buffer->meta.cursor.written = 0;
    buffer->meta.cursor.bit = 0;
    buffer->meta.cursor.byte = 0;
    buffer->meta.flags = 0;

    return COREX_TRUE;
}


byte _read_buffer_impl(u64 count, void* bytes, Buffer* buffer) {
    if (!BUFFER_VALID(buffer) || !BUFFER_CAN_READ(buffer) || !count || !bytes) {
        return COREX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }

    COREX_FOR(u64, b, 0, count, 1) {
        if (buffer->meta.cursor.byte + b >= buffer->meta.size) break;
        *((byte*)bytes + b) = *(((byte*)buffer->data + buffer->meta.cursor.byte) + b);
    }
    
    buffer->meta.cursor.byte += (COREX_GET_BITS(buffer->meta.flags, BUFFER_FLAG_AUTOCURSOR)) ? count : 0;
    return COREX_TRUE;
}

byte _write_buffer_impl(u64 count, void* bytes, Buffer* buffer) {
    if (!BUFFER_VALID(buffer) || !BUFFER_CAN_WRITE(buffer) || !count || !bytes) {
        return COREX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }

    if (count > buffer->meta.size - buffer->meta.cursor.written) {
        return COREX_FALSE;  // error: not enough space for this write!
    }
    
    COREX_FOR(u64, b, 0, count, 1) {
        if (buffer->meta.cursor.byte + b >= buffer->meta.size) break;
        *(((byte*)buffer->data + buffer->meta.cursor.byte) + b) = *((byte*)bytes + b);
    }
    
    buffer->meta.cursor.byte += (COREX_GET_BITS(buffer->meta.flags, BUFFER_FLAG_AUTOCURSOR)) ? count : 0;
    buffer->meta.cursor.written+=count;
    return COREX_TRUE;
}

byte _peek_impl(i64 offset, u64 count, void* bytes, Buffer* buffer) {
    if (!BUFFER_VALID(buffer) || !BUFFER_CAN_READ(buffer)) return COREX_FALSE;
    if (!bytes || !count ||
        count > buffer->meta.size ||
        buffer->meta.cursor.byte + offset < 0 ||
        buffer->meta.cursor.byte + offset > (i64)(buffer->meta.size)
    ) return COREX_FALSE;
    u64 restore = buffer->meta.cursor.byte;
    buffer->meta.cursor.byte += offset;
    byte result = corex->genx.buffer.read(count, bytes, buffer);
    buffer->meta.cursor.byte = restore;
    return result;
}

byte _inject_impl(i64 offset, u64 count, void* bytes, Buffer* buffer) {
    if (!BUFFER_VALID(buffer) || !BUFFER_CAN_WRITE(buffer)) return COREX_FALSE;
    if (!bytes || !count ||
        count > buffer->meta.size ||
        buffer->meta.cursor.byte + offset < 0 ||
        buffer->meta.cursor.byte + offset > (i64)(buffer->meta.size)
    ) return COREX_FALSE;
    u64 restore = buffer->meta.cursor.byte;
    buffer->meta.cursor.byte += offset;
    byte result = corex->genx.buffer.write(count, bytes, buffer);
    buffer->meta.cursor.byte = restore;
    return result;
}


byte _tell_impl(Buffer_Cursor cursor, Buffer* buffer) {
    if (!BUFFER_VALID(buffer) || cursor < 0 || cursor > BUFFER_CURSOR_TYPES) return COREX_FALSE;
    switch (cursor) {
        case BUFFER_CURSOR_BIT: {
            return buffer->meta.cursor.bit;
        }
        case BUFFER_CURSOR_BYTE: {
            return buffer->meta.cursor.byte;
        }
        case BUFFER_CURSOR_NONE:    // fall through to default
        default: break;
    }
    return COREX_TRUE;
}

byte _rewind_impl(Buffer_Cursor cursor, Buffer* buffer) {
    if (!BUFFER_VALID(buffer) || !BUFFER_CAN_WRITE(buffer)) return COREX_FALSE;
    if (cursor < 0 || cursor > BUFFER_CURSOR_TYPES) return COREX_FALSE;
    switch (cursor) {
        case BUFFER_CURSOR_BIT: {
            buffer->meta.cursor.bit = 0;
            return COREX_TRUE;
        }
        case BUFFER_CURSOR_BYTE: {
            buffer->meta.cursor.byte = 0;
            return COREX_TRUE;
        }
        case BUFFER_CURSOR_NONE:    // fall through to default
        case BUFFER_CURSOR_TYPES:   // fall through to default
        default: return COREX_FALSE;
    }
    return COREX_TRUE;
}

byte _seek_impl(Buffer_Cursor cursor, u64 offset, Buffer* buffer) {
    if (!BUFFER_VALID(buffer) || !BUFFER_CAN_WRITE(buffer)) return COREX_FALSE;
    if (offset < 0 || cursor < 0 || cursor > BUFFER_CURSOR_TYPES) return COREX_FALSE;
    switch (cursor) {
        case BUFFER_CURSOR_BIT: {
            if (offset >= 8) return COREX_FALSE;
            buffer->meta.cursor.bit = offset;
            return COREX_TRUE;
        }
        case BUFFER_CURSOR_BYTE: {
            if (offset >= buffer->meta.size) return COREX_FALSE;
            buffer->meta.cursor.byte = offset;
            return COREX_TRUE;
        }
        case BUFFER_CURSOR_NONE:    // fall through to default
        case BUFFER_CURSOR_TYPES:   // fall through to default
        default: return COREX_FALSE;
    }
}

byte _seekr_impl(Buffer_Cursor cursor, i64 offset, Buffer* buffer) {
    if (!BUFFER_VALID(buffer) || !BUFFER_CAN_WRITE(buffer)) return COREX_FALSE;
    if (cursor < 0 || cursor > BUFFER_CURSOR_TYPES) return COREX_FALSE;
    switch (cursor) {
        case BUFFER_CURSOR_BIT: {
            if (
                offset >= 8 ||
                buffer->meta.cursor.bit + offset < 0 ||
                buffer->meta.cursor.bit + offset >= 8
            ) return COREX_FALSE;
            buffer->meta.cursor.bit += offset;
            return COREX_TRUE;
        }
        case BUFFER_CURSOR_BYTE: {
            if (
                offset >= (i64)(buffer->meta.size) ||
                buffer->meta.cursor.byte + offset < 0 ||
                buffer->meta.cursor.byte + offset >= (i64)(buffer->meta.size)
            ) return COREX_FALSE;
            buffer->meta.cursor.byte += offset;
            return COREX_TRUE;
        }
        case BUFFER_CURSOR_NONE:    // fall through to default
        case BUFFER_CURSOR_TYPES:   // fall through to default
        default: return COREX_FALSE;
    }
}


byte _set_flag_impl(Buffer_Flag flag, Buffer* buffer) {
    if (!BUFFER_VALID(buffer)) {
        return COREX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }
    buffer->meta.flags = COREX_SET_BITS(buffer->meta.flags, flag);
    return COREX_TRUE;
}

byte _has_flag_impl(Buffer_Flag flag, Buffer* buffer) {
    if (!BUFFER_VALID(buffer)) {
        return COREX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }
    return (COREX_GET_BITS(buffer->meta.flags, flag)) ? COREX_TRUE : COREX_FALSE;
}

byte _rem_flag_impl(Buffer_Flag flag, Buffer* buffer) {
    if (!BUFFER_VALID(buffer)) {
        return COREX_FALSE;  // error: null ptr/buffer in a "bad" state!
    }
    buffer->meta.flags = COREX_REM_BITS(buffer->meta.flags, flag);
    return COREX_TRUE;
}


byte _corex_init_genx(void) {
    if (!corex) return COREX_FALSE; // error: null ptr!
    if (corex->genx.init == COREX_TRUE) return COREX_TRUE;    // redundant call: Genx API already initialized!

    if (corex->memx.init == COREX_FALSE) {
        printf("Corex Memx API not initialized!\n");
        return COREX_FALSE; // error: failed to initialize Memx API!
    }
    
    if (corex->dsx.init == COREX_FALSE) {
        printf("Corex Dsx API not initialized!\n");
        return COREX_FALSE; // error: failed to initialize Dsx API!
    }

    corex->genx.buffer.create = _create_buffer_impl;
    corex->genx.buffer.reset = _reset_impl;
    corex->genx.buffer.destroy = _destroy_buffer_impl;
    
    corex->genx.buffer.read = _read_buffer_impl;
    corex->genx.buffer.write = _write_buffer_impl;
    
    corex->genx.buffer.peek = _peek_impl;
    corex->genx.buffer.inject = _inject_impl;
    
    corex->genx.buffer.tell = _tell_impl;
    corex->genx.buffer.rewind = _rewind_impl;
    corex->genx.buffer.seek = _seek_impl;
    corex->genx.buffer.seekr = _seekr_impl;
    
    corex->genx.buffer.set_flag = _set_flag_impl;
    corex->genx.buffer.has_flag = _has_flag_impl;
    corex->genx.buffer.rem_flag = _rem_flag_impl;

    corex->genx.bin.depth8 = _depth8_impl;
    corex->genx.bin.pack8 = _pack8_impl;
    corex->genx.bin.unpack8 = _unpack8_impl;

    corex->meta.usage.genx = sizeof(Genx);
	corex->genx.init = COREX_TRUE;
    return COREX_TRUE;
}

void _corex_cleanup_genx(void) {
    if (corex->genx.init == COREX_FALSE) return;    // error: Genx API not initialized!
	corex->meta.usage.apis &= ~COREX_GENX;
	corex->genx.init = COREX_FALSE;
    corex->meta.usage.genx = 0;
	corex->genx	= (Genx){0};
}
