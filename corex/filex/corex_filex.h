#ifndef __COREX_FILEX_H__
#define __COREX_FILEX_H__

#include <corex/corex_def.h>

typedef struct Filex {
    byte (*exists)(cstr path);
    u64 (*size)(cstr path);
    byte (*remove)(cstr path);
    str (*read)(cstr path, u64 size);
    byte (*copy)(cstr src, cstr dest, u64 size);
    byte (*copyb)(cstr src, cstr dest, u64 size);
    byte (*readb)(cstr path, u64 size, void* out);
    byte (*write)(cstr path, cstr lines, byte preserve);
    byte (*append)(cstr path, cstr lines, byte newline);
    byte (*writeb)(cstr path, u64 size, byte preserve, void* ptr);
    byte (*appendb)(cstr path, u64 size, byte newline, void* ptr);
    byte (*process)(cstr path, u64 lines, void (*callback)(cstr line));
    byte init;
} Filex;

byte _corex_init_filex(void);
void _corex_cleanup_filex(void);

#endif  // COREX_FILEX_H