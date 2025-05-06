#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <include/SSDK/defines.h>

typedef struct SaneModule {
    u64 calls;
    u64 size;
    str name;
    u8 mask;
} SaneModule;

#endif  // __GLOBALS_H__