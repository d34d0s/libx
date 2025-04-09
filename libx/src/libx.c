#include <libx/include/libx.h>

Libx* libx = NULL;

u8 libx_init(Libx_Api mask) {
    if (libx != NULL) return LIBX_TRUE; // redundant call: libx struct initialized!

    if (!mask) return LIBX_FALSE;

    libx = malloc(sizeof(Libx));
	if (!libx) return NULL;	// error: out of memory!

    libx->meta.usage.apis = mask;

    if (mask == LIBX_ALL) {
        if (!libx_init_memx()   ||
            !libx_init_mathx()  ||
            !libx_init_dsx()    ||
            !libx_init_genx()   ||
            !libx_init_ecsx()   ||
            !libx_init_filex()) {
            return LIBX_FALSE;
        }
        return LIBX_TRUE;
    }

    LIBX_INIT_IF(LIBX_MEMX, libx_init_memx);
    LIBX_INIT_IF(LIBX_MATHX, libx_init_mathx);
    LIBX_INIT_IF(LIBX_DSX, libx_init_dsx);
    LIBX_INIT_IF(LIBX_GENX, libx_init_genx);
    LIBX_INIT_IF(LIBX_FILEX, libx_init_filex);
    LIBX_INIT_IF(LIBX_ECSX, libx_init_ecsx);

    return LIBX_TRUE;
}

u8 libx_deinit(Libx_Api mask) {
    if (!libx) return LIBX_FALSE;
    if (!mask) return LIBX_FALSE;

    if (mask == LIBX_ALL) {
        if (!libx_cleanup_memx()   ||
            !libx_cleanup_mathx()  ||
            !libx_cleanup_dsx()    ||
            !libx_cleanup_genx()   ||
            !libx_cleanup_ecsx()   ||
            !libx_cleanup_filex()  ||) {
            return LIBX_FALSE;
        }
        return LIBX_TRUE;
    }
   
    LIBX_CLEANUP_IF(LIBX_MEMX, libx_init_memx);
    LIBX_CLEANUP_IF(LIBX_MATHX, libx_init_mathx);
    LIBX_CLEANUP_IF(LIBX_DSX, libx_init_dsx);
    LIBX_CLEANUP_IF(LIBX_GENX, libx_init_genx);
    LIBX_CLEANUP_IF(LIBX_FILEX, libx_init_filex);
    LIBX_CLEANUP_IF(LIBX_ECSX, libx_init_ecsx);

    return LIBX_TRUE;
}

u8 libx_cleanup(void) {
    if (!libx) return LIBX_TRUE;
    libx_deinit(libx->meta.usage.apis);
    free(libx);
    libx = NULL;
    return LIBX_TRUE;
}
