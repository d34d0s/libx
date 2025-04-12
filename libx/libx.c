#include <libx/libx.h>
#include <libx/libx_version.h>

Libx* libx = NULL;

void _dump_impl(void) {
    u64 total = libx->meta.usage.memx + libx->meta.usage.dsx + libx->meta.usage.genx + libx->meta.usage.ecsx + libx->meta.usage.mathx + libx->meta.usage.filex;
    printf("---Libx %s---\n", libx->meta.version.string);
    printf("\"%s\"\n", libx->meta.version.quote);
    printf("|Memx  %d: %lluB\n", libx->memx.init, libx->meta.usage.memx);
    printf("|Dsx   %d: %lluB\n", libx->dsx.init, libx->meta.usage.dsx);
    printf("|Genx  %d: %lluB\n", libx->genx.init, libx->meta.usage.genx);
    printf("|Ecsx  %d: %lluB\n", libx->ecsx.init, libx->meta.usage.ecsx);
    printf("|Mathx %d: %lluB\n", libx->mathx.init, libx->meta.usage.mathx);
    printf("|Filex %d: %lluB", libx->filex.init, libx->meta.usage.filex);
    printf("\n-----Total %lluB-----\n", total);
}

u8 libx_init(Libx_Api mask) {
    if (!mask) return LIBX_FALSE;

    if (libx == NULL) {
        libx = malloc(sizeof(Libx));
        if (!libx) return LIBX_FALSE;	// error: out of memory!

        libx->meta.version.major = LIBX_VERSION_MAJOR;
        libx->meta.version.minor = LIBX_VERSION_MINOR;
        libx->meta.version.patch = LIBX_VERSION_PATCH;
        libx->meta.version.string = LIBX_VERSION_STRING;
        libx->meta.version.quote = "Who really reads these?";
        
        libx->meta.usage.apis = 0;
        libx->meta.usage.memx = 0;
        libx->meta.usage.mathx = 0;
        libx->meta.usage.dsx = 0;
        libx->meta.usage.genx = 0;
        libx->meta.usage.filex = 0;
        libx->meta.usage.ecsx = 0;
        
        libx->memx.init = LIBX_FALSE;
        libx->mathx.init = LIBX_FALSE;
        libx->dsx.init = LIBX_FALSE;
        libx->genx.init = LIBX_FALSE;
        libx->filex.init = LIBX_FALSE;
        libx->ecsx.init = LIBX_FALSE;

        libx->dump = _dump_impl;
    }

    // check if any new modules were requested
    Libx_Api new_mask = mask & ~libx->meta.usage.apis;
    if (!new_mask) return LIBX_TRUE; // error: requested apis already initialized!

    libx->meta.usage.apis |= new_mask;

    if ((new_mask & LIBX_ALL) == LIBX_ALL) {
        if (!_libx_init_memx()   ||
            !_libx_init_mathx()  ||
            !_libx_init_dsx()    ||
            !_libx_init_genx()   ||
            !_libx_init_ecsx()   ||
            !_libx_init_filex()) {
            return LIBX_FALSE;
        }
        return LIBX_TRUE;
    }

    if (new_mask & LIBX_MEMX)  { if (!_libx_init_memx())   return LIBX_FALSE; }
    if (new_mask & LIBX_MATHX) { if (!_libx_init_mathx())  return LIBX_FALSE; }
    if (new_mask & LIBX_DSX)   { if (!_libx_init_dsx())    return LIBX_FALSE; }
    if (new_mask & LIBX_GENX)  { if (!_libx_init_genx())   return LIBX_FALSE; }
    if (new_mask & LIBX_FILEX) { if (!_libx_init_filex())  return LIBX_FALSE; }
    if (new_mask & LIBX_ECSX)  { if (!_libx_init_ecsx())   return LIBX_FALSE; }

    return LIBX_TRUE;
}

u8 libx_deinit(Libx_Api mask) {
    if (!libx) return LIBX_FALSE;
    if (!mask) return LIBX_FALSE;

    if (mask == LIBX_ALL) {
        _libx_cleanup_ecsx();
        _libx_cleanup_filex();
        _libx_cleanup_genx();
        _libx_cleanup_dsx();
        _libx_cleanup_mathx();
        _libx_cleanup_memx();
        return LIBX_TRUE;
    }
   
    if (mask & LIBX_ECSX)  { _libx_cleanup_ecsx(); }
    if (mask & LIBX_FILEX) { _libx_cleanup_filex(); }
    if (mask & LIBX_GENX)  { _libx_cleanup_genx(); }
    if (mask & LIBX_DSX)   { _libx_cleanup_dsx(); }
    if (mask & LIBX_MATHX) { _libx_cleanup_mathx(); }
    if (mask & LIBX_MEMX)  { _libx_cleanup_memx(); }
    
    return LIBX_TRUE;
}

u8 libx_cleanup(void) {
    if (!libx) return LIBX_TRUE;
    libx_deinit(libx->meta.usage.apis);
    free(libx);
    libx = NULL;
    return LIBX_TRUE;
}
