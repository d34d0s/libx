#include <libx/libx.h>

u8 _libx_init_genx(void) {
    if (!libx) return LIBX_FALSE; // error: null ptr!
    if (libx->genx.init == LIBX_TRUE) return LIBX_TRUE;    // redundant call: Genx API already initialized!

    if (libx->memx.init == LIBX_FALSE) {
        printf("Libx Memx API not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize Memx API!
    }
    
    if (libx->dsx.init == LIBX_FALSE) {
        printf("Libx Dsx API not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize Dsx API!
    }

    libx->genx.buffer.create;
    libx->genx.buffer.read;
    libx->genx.buffer.write;
    libx->genx.buffer.reset;
    libx->genx.buffer.destroy;
    libx->genx.buffer.compress;
    libx->genx.buffer.serialize;

	libx->genx.init = LIBX_TRUE;
    return LIBX_TRUE;
}

void _libx_cleanup_genx(void) {
    if (libx->genx.init == LIBX_FALSE) return;    // error: Genx API not initialized!
	libx->genx.init = LIBX_FALSE;
	libx->genx	= (Genx){0};
}
