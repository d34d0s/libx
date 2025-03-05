#pragma once

#include "stdx_def.h"

typedef struct _stdx_fileio_api {
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
} _stdx_fileio_api;
extern _stdx_fileio_api* fileio_api;

STDX_API u8 stdx_init_fileio(void);
STDX_API void stdx_cleanup_fileio(void);
