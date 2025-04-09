#ifndef LIBX_FILEX_H
#define LIBX_FILEX_H

#include <libx/include/libx_def.h>

typedef struct Filex {
    u8 init;
    u8 (*exists)(cstr path);
    u64 (*size)(cstr path);
    u8 (*remove)(cstr path);
    str (*read)(cstr path, u64 size);
    u8 (*copy)(cstr src, cstr dest, u64 size);
    u8 (*copyb)(cstr src, cstr dest, u64 size);
    u8 (*readb)(cstr path, u64 size, void* out);
    u8 (*write)(cstr path, cstr lines, u8 preserve);
    u8 (*append)(cstr path, cstr lines, u8 newline);
    u8 (*writeb)(cstr path, u64 size, u8 preserve, void* ptr);
    u8 (*appendb)(cstr path, u64 size, u8 newline, void* ptr);
    u8 (*process)(cstr path, u64 lines, void (*callback)(cstr line));
} Filex;

LIBX_EXPOSE u8 libx_init_filex(void);
LIBX_EXPOSE void libx_cleanup_filex(void);

#endif  // LIBX_FILEX_H