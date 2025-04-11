#ifndef __LIBX_GENX_H__
#define __LIBX_GENX_H__

#include <libx/libx_def.h>

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
    BUFFER_SIZE_1B =    0b00000001,
    BUFFER_SIZE_2B =    0b00000010,
    BUFFER_SIZE_4B =    0b00000100,
    BUFFER_SIZE_8B =    0b00001000,
    BUFFER_SIZE_16B =   0b00010000,
    BUFFER_SIZE_32B =   0b00100000,
    BUFFER_SIZE_64B =   0b01000000,
    BUFFER_SIZE_MAX =  0b10000000
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
    Buffer_Type type;
    Buffer_Size size;
    Buffer_Access access;
} Buffer_Layout;

typedef struct Buffer {
    void* data;
    u8 desc[8];
} Buffer;

typedef struct Genx {
    struct bin {
        u8 (*depth8)(u8 value);
        u8 (*pack8)(u8 value1, u8 value2);
        u8* (*unpack8)(u8 value, u8 depth, u8* out);
    } bin;

    struct buffer {
        u8 (*create)(Buffer_Layout layout, Buffer* buffer);
        u8 (*read)(u64 count, void* bytes, Buffer* buffer);
        u8 (*write)( u64 count, void* bytes, Buffer* buffer);
        u8 (*bit_cursor)(u8 bit, Buffer* buffer);
        u8 (*byte_cursor)(u64 byte, Buffer* buffer);
        u8 (*set_flag)(Buffer_Flag flag, Buffer* buffer);
        u8 (*has_flag)(Buffer_Flag flag, Buffer* buffer);
        u8 (*rem_flag)(Buffer_Flag flag, Buffer* buffer);
        u8 (*set_field)(u8 field, u8 value, Buffer* buffer);
        u8 (*get_field)(u8 field, Buffer* buffer);
        u8 (*reset)(Buffer* buffer);
        u8 (*destroy)(Buffer* buffer);
    } buffer;

    u8 init;
} Genx;

u8 _libx_init_genx(void);
void _libx_cleanup_genx(void);

#endif  // LIBX_GENX_H