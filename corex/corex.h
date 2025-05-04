/*
----------------------------------------------------------
Corex Runtime Library
----------------------------------------------------------

Modules:
    - MEMX   : Memory utilities
    - MATHX  : Math library
    - DSX    : Data structures
    - GENX   : General utilities
    - FILEX  : File I/O
    - ECSX   : ECS framework (Entity-Component-System)

Usage:
    corex_init(COREX_MEMX | COREX_DSX);
    ...
    corex_cleanup();

corex is a singleton pointer to the active runtime.

----------------------------------------------------------
*/

#ifndef __COREX_H__
#define __COREX_H__

#include <corex/memx/corex_memx.h>
#include <corex/dsx/corex_dsx.h>
#include <corex/strx/corex_strx.h>
#include <corex/genx/corex_genx.h>
#include <corex/ecsx/corex_ecsx.h>
#include <corex/mathx/corex_mathx.h>
#include <corex/filex/corex_filex.h>

typedef enum Corex_Api {
    COREX_NOAPI  = 0,
    COREX_MEMX   = (1 << 0),
    COREX_MATHX  = (1 << 1),
    COREX_DSX    = (1 << 2),
    COREX_STRX   = (1 << 3),
    COREX_GENX   = (1 << 4),
    COREX_FILEX  = (1 << 5),
    COREX_ECSX   = (1 << 6),
    COREX_ALL    = COREX_MEMX|COREX_MATHX|COREX_DSX|COREX_STRX|COREX_GENX|COREX_FILEX|COREX_ECSX,
} Corex_Api;

typedef struct Corex_Metadata {
    struct version {
        u32 major, minor, patch;
        cstr string;
        cstr quote;         // a funny random quote (might look into a string lib as well as generating random quote strings :o Corex::strx ?!?!?) 
    } version;

    struct usage {
        u8 apis;            // bitmask passed to corex_init
        u64 memx;
        u64 dsx;
        u64 strx;
        u64 genx;
        u64 ecsx;
        u64 mathx;
        u64 filex;
    } usage;
} Corex_Metadata;

typedef struct Corex {
    Memx memx;
    Dsx dsx;
    Strx strx;
    Genx genx;
    Ecsx ecsx;
    Mathx mathx;
    Filex filex;
    Corex_Metadata meta;
    void (*dump)(void);
} Corex;
extern Corex* corex;

COREX_EXPOSE byte corex_init(Corex_Api mask);
COREX_EXPOSE byte corex_deinit(Corex_Api mask);
COREX_EXPOSE byte corex_cleanup(void);

#endif  // COREX_H