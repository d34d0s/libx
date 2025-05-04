#ifndef __COREX_STRX_H__
#define __COREX_STRX_H__

#include <corex/corex_def.h>

#define COREX_STR_MAX (COREX_MB * 4)

typedef struct String {
    byte* buffer;
    Array_Head meta;
} String;

typedef struct Strx {
    byte (*create)(u32 size, String* string);
    byte (*destroy)(String* string);
    
    u32 (*len)(String* string);
    byte (*grow)(u32 count, byte* chars, String* string);
    
    byte (*copy)(String* src, String* dest);
    byte (*ncopy)(u32 count, String* src, String* dest);
    byte init;
} Strx;

byte _corex_init_strx(void);
void _corex_cleanup_strx(void);

#endif // __COREX_STRX_H__