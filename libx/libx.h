/*
----------------------------------------------------------
Libx Runtime Library
----------------------------------------------------------

Modules:
    - MEMX   : Memory utilities
    - MATHX  : Math library
    - DSX    : Data structures
    - GENX   : General utilities
    - FILEX  : File I/O
    - ECSX   : ECS framework (Entity-Component-System)

Usage:
    libx_init(LIBX_MEMX | LIBX_DSX);
    ...
    libx_cleanup();

libx is a singleton pointer to the active runtime.

----------------------------------------------------------
*/

#ifndef __LIBX_H__
#define __LIBX_H__

#include <libx/memx/libx_memx.h>
#include <libx/dsx/libx_dsx.h>
#include <libx/genx/libx_genx.h>
#include <libx/ecsx/libx_ecsx.h>
#include <libx/mathx/libx_mathx.h>
#include <libx/filex/libx_filex.h>

typedef enum Libx_Api {
    LIBX_NOAPI  = 0,
    LIBX_MEMX   = (1 << 0),
    LIBX_MATHX  = (1 << 1),
    LIBX_DSX    = (1 << 2),
    LIBX_GENX   = (1 << 3),
    LIBX_FILEX  = (1 << 4),
    LIBX_ECSX   = (1 << 5),
    LIBX_ALL    = LIBX_MEMX|LIBX_MATHX|LIBX_DSX|LIBX_GENX|LIBX_FILEX|LIBX_ECSX,
} Libx_Api;

typedef struct Libx_Metadata {
    struct version {
        u32 major, minor, patch;
        cstr string;
        cstr quote;         // a funny random quote (might look into a string lib as well as generating random quote strings :o Libx::strx ?!?!?) 
    } version;

    struct usage {
        u8 apis;            // bitmask passed to libx_init
        u64 memx;
        u64 dsx;
        u64 genx;
        u64 ecsx;
        u64 mathx;
        u64 filex;
    } usage;
} Libx_Metadata;

typedef struct Libx {
    Memx memx;
    Dsx dsx;
    Genx genx;
    Ecsx ecsx;
    Mathx mathx;
    Filex filex;
    Libx_Metadata meta;

    void (*dump)(void);
} Libx;
extern Libx* libx;

LIBX_EXPOSE u8 libx_init(Libx_Api mask);
LIBX_EXPOSE u8 libx_deinit(Libx_Api mask);
LIBX_EXPOSE u8 libx_cleanup(void);

#endif  // LIBX_H