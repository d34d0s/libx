#ifndef LIBX_GENX_H
#define LIBX_GENX_H

#include <libx/include/libx_def.h>

typedef enum Buffer_Field {
    BUFFER_NOFIELD      =   0x0000,
    BUFFER_WRITTEN      =   0x0001,
    BUFFER_TYPE         =   0x0002,
    BUFFER_BIT          =   0x0003,
    BUFFER_BYTE         =   0x0004,
    BUFFER_FIELDS
} Buffer_Field;

typedef enum Buffer_Type {
    BUFFER_DEFAULT  = 0x0000,
    BUFFER_4B       = 0x0001,
    BUFFER_8B       = 0x0002,
    BUFFER_16B      = 0x0003,
    BUFFER_64B      = 0x0004,
    BUFFER_128B     = 0x0005,
    BUFFER_TYPES
} Buffer_Type;

typedef struct Buffer {
    // 132 byte max
    union {
        u8 b4[4];
        u8 b8[8];
        u8 b16[16];
        u8 b32[32];
        u8 b64[64];
        u8 b128[128];
    } data;
    u8 desc[4];
} Buffer;

typedef struct Genx {
    u8 init;
    struct buffer {
        u8 (*create)(Buffer_Type type, Buffer* buffer);
        u8 (*read)(Buffer* buffer);
        u8 (*write)(Buffer* buffer);
        u8 (*reset)(Buffer* buffer);
        u8 (*destroy)(Buffer* buffer);
        u8 (*compress)(Buffer* buffer);
        u8 (*serialize)(Buffer* buffer);
    } buffer;
} Genx;

LIBX_EXPOSE u8 libx_init_genx(void);
LIBX_EXPOSE void libx_cleanup_genx(void);

#endif  // LIBX_GENX_H