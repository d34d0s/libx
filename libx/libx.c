#include <libx/libx.h>
#include <libx/libx_version.h>

Libx* libx = NULL;

u8 libx_init(Libx_Api mask) {
    if (!mask) return LIBX_FALSE;

    if (libx == NULL) {
        libx = malloc(sizeof(Libx));
        if (!libx) return LIBX_FALSE;	// error: out of memory!

        libx->meta.version.major = LIBX_VERSION_MAJOR;
        libx->meta.version.minor = LIBX_VERSION_MINOR;
        libx->meta.version.patch = LIBX_VERSION_PATCH;
        libx->meta.version.string = LIBX_VERSION_STRING;
        libx->meta.version.quote = "Look mom, another runtime lib!";
        
        libx->meta.usage.apis = 0;
        libx->memx.init = LIBX_FALSE;
        libx->mathx.init = LIBX_FALSE;
        libx->dsx.init = LIBX_FALSE;
        libx->genx.init = LIBX_FALSE;
        libx->filex.init = LIBX_FALSE;
        libx->ecsx.init = LIBX_FALSE;
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
