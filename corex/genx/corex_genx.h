#ifndef __LIBX_GENX_H__
#define __LIBX_GENX_H__

#include <corex/corex_def.h>

typedef enum Buffer_Type {
    BUFFER_TYPE_NONE,
    BUFFER_TYPE_PTR,
    BUFFER_TYPE_CHAR,
    BUFFER_TYPE_UCHAR,
    BUFFER_TYPE_SCHAR,
    BUFFER_TYPE_INT8,
    BUFFER_TYPE_INT16,
    BUFFER_TYPE_INT32,
    BUFFER_TYPE_INT64,
    BUFFER_TYPE_UINT8,
    BUFFER_TYPE_UINT16,
    BUFFER_TYPE_UINT32,
    BUFFER_TYPE_UINT64,
    BUFFER_TYPE_FLOAT32,
    BUFFER_TYPE_FLOAT64,
    BUFFER_TYPES
} Buffer_Type;

typedef enum Buffer_Size {
    BUFFER_SIZE_NONE,
    BUFFER_SIZE_1B =    0x0001,
    BUFFER_SIZE_2B =    0x0002,
    BUFFER_SIZE_4B =    0x0004,
    BUFFER_SIZE_8B =    0x0008,
    BUFFER_SIZE_16B =   0x0010,
    BUFFER_SIZE_32B =   0x0020,
    BUFFER_SIZE_64B =   0x0040,
    BUFFER_SIZE_128B =  0x0080,
    BUFFER_SIZE_256B =  0x0100,
    BUFFER_SIZE_512B =  0x0200,
    BUFFER_SIZE_1KB =   0x0400,
    BUFFER_SIZE_2KB =   0x0800,
    BUFFER_SIZE_4KB =   0x1000,
    BUFFER_SIZE_MAX =   0x1000
} Buffer_Size;

typedef enum Buffer_Access {
    BUFFER_ACCESS_NONE,
    BUFFER_ACCESS_R,
    BUFFER_ACCESS_RW,
    BUFFER_ACCESS_TYPES
} Buffer_Access;

typedef enum Buffer_State {
    BUFFER_STATE_NONE,
    BUFFER_STATE_ALLOC,
    BUFFER_STATE_FREE,
    BUFFER_STATE_VALID,
    BUFFER_STATE_INVALID,
    BUFFER_STATES
} Buffer_State;

typedef enum Buffer_Flag {
    BUFFER_FLAG_AUTOCURSOR,
    BUFFER_FLAGS
} Buffer_Flag;

typedef enum Buffer_Cursor {
    BUFFER_CURSOR_NONE,
    BUFFER_CURSOR_BIT,
    BUFFER_CURSOR_BYTE,
    BUFFER_CURSOR_TYPES,
} Buffer_Cursor;

typedef enum Buffer_Field {
    BUFFER_FIELD_NONE,
    BUFFER_FIELD_TYPE,
    BUFFER_FIELD_SIZE,
    BUFFER_FIELD_ACCESS,
    BUFFER_FIELD_STATE,
    BUFFER_FIELD_WRITTEN,
    BUFFER_FIELD_BIT,
    BUFFER_FIELD_BYTE,
    BUFFER_FIELD_FLAGS,
    BUFFER_FIELDS
} Buffer_Field;

typedef struct Buffer_Layout {
    u8 type;
    u64 size;
    u8 access;
} Buffer_Layout;

typedef struct Buffer {
    void* data;
    struct meta {
        struct cursor {
            u64 written;
            u64 byte;
            u8 bit;
        } cursor;
        u64 size;
        u8 flags;
        u8 type;
        u8 state;
        u8 access;
    } meta;
} Buffer;

typedef struct Genx {
    struct bin {
        u8 (*depth8)(u8 value);
        u8 (*pack8)(u8 value1, u8 value2);
        u8* (*unpack8)(u8 value, u8 depth, u8* out);
    } bin;

    struct buffer {
        u8 (*create)(Buffer_Layout layout, Buffer* buffer);
        u8 (*reset)(Buffer* buffer);
        u8 (*destroy)(Buffer* buffer);
        
        u8 (*read)(u64 count, void* bytes, Buffer* buffer);
        u8 (*write)( u64 count, void* bytes, Buffer* buffer);
        u8 (*peek)(u64 offset, u64 count, void* bytes, Buffer* buffer);
        u8 (*inject)(u64 offset, u64 count, void* bytes, Buffer* buffer);
        
        u8 (*rewind)(Buffer_Cursor cursor, Buffer* buffer);
        u8 (*seek)(Buffer_Cursor cursor, u64 byte, Buffer* buffer);
        u8 (*seekr)(Buffer_Cursor cursor, u64 offset, Buffer* buffer);
        
        u8 (*set_flag)(Buffer_Flag flag, Buffer* buffer);
        u8 (*has_flag)(Buffer_Flag flag, Buffer* buffer);
        u8 (*rem_flag)(Buffer_Flag flag, Buffer* buffer);
    } buffer;

    u8 init;
} Genx;

u8 _corex_init_genx(void);
void _corex_cleanup_genx(void);

#endif  // LIBX_GENX_H