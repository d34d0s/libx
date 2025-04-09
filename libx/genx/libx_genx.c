#include <libx/include/libx.h>

u8 libx_init_genx(void) {
    if (!libx) return LIBX_FALSE; // error: null ptr!
    if (libx->genx.init) return LIBX_TRUE;    // redundant call: Genx API already initialized!

    libx->genx.buffer.create;
    libx->genx.buffer.read;
    libx->genx.buffer.write;
    libx->genx.buffer.reset;
    libx->genx.buffer.destroy;
    libx->genx.buffer.compress;
    libx->genx.buffer.serialize;

    return LIBX_TRUE;
}

void libx_cleanup_genx(void) {
    if (!libx->genx.init) return;    // error: Genx API not initialized!
	libx->genx	= (Genx){NULL};       
}
