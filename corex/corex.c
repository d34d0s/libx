#include <corex/corex.h>
#include <corex/corex_version.h>

Corex* corex = NULL;

byte (*init_table[])(void) = {
    _corex_init_memx,
    _corex_init_mathx,
    _corex_init_dsx,
    _corex_init_strx,
    _corex_init_genx,
    _corex_init_filex,
    _corex_init_ecsx
};

void (*cleanup_table[])(void) = {
    _corex_cleanup_memx,
    _corex_cleanup_mathx,
    _corex_cleanup_dsx,
    _corex_cleanup_strx,
    _corex_cleanup_genx,
    _corex_cleanup_filex,
    _corex_cleanup_ecsx
};

void _dump_impl(void) {
    u64 total = corex->meta.usage.memx + corex->meta.usage.dsx + corex->meta.usage.genx + corex->meta.usage.ecsx + corex->meta.usage.mathx + corex->meta.usage.filex;
    printf("---Corex %s---\n", corex->meta.version.string);
    printf("\"%s\"\n", corex->meta.version.quote);
    printf("|Memx  %d: %lluB\n", corex->memx.init, corex->meta.usage.memx);
    printf("|Dsx   %d: %lluB\n", corex->dsx.init, corex->meta.usage.dsx);
    printf("|Genx  %d: %lluB\n", corex->genx.init, corex->meta.usage.genx);
    printf("|Ecsx  %d: %lluB\n", corex->ecsx.init, corex->meta.usage.ecsx);
    printf("|Mathx %d: %lluB\n", corex->mathx.init, corex->meta.usage.mathx);
    printf("|Filex %d: %lluB", corex->filex.init, corex->meta.usage.filex);
    printf("\n-----Total %lluB-----\n", total);
}

byte corex_init(Corex_Api mask) {
    if (!mask) return COREX_FALSE;

    if (corex == NULL) {
        corex = malloc(sizeof(Corex));
        if (!corex) return COREX_FALSE;	// error: out of memory!

        corex->meta.version.major = COREX_VERSION_MAJOR;
        corex->meta.version.minor = COREX_VERSION_MINOR;
        corex->meta.version.patch = COREX_VERSION_PATCH;
        corex->meta.version.string = COREX_VERSION_STRING;
        corex->meta.version.quote = "Welcome to Li-, uh, Corex!";
        
        corex->meta.usage.apis = 0;
        corex->meta.usage.memx = 0;
        corex->meta.usage.mathx = 0;
        corex->meta.usage.dsx = 0;
        corex->meta.usage.genx = 0;
        corex->meta.usage.filex = 0;
        corex->meta.usage.ecsx = 0;
        
        corex->memx.init = COREX_FALSE;
        corex->mathx.init = COREX_FALSE;
        corex->dsx.init = COREX_FALSE;
        corex->genx.init = COREX_FALSE;
        corex->filex.init = COREX_FALSE;
        corex->ecsx.init = COREX_FALSE;

        corex->dump = _dump_impl;
    }

    // check if any new modules were requested
    Corex_Api new_mask = mask & ~corex->meta.usage.apis;
    if (!new_mask) return COREX_TRUE; // error: requested apis already initialized!

    corex->meta.usage.apis |= new_mask;

    COREX_FORI(0, 6, 1) {
        if (new_mask & (1 << i)) {
            if (init_table[i] == NULL) return COREX_FALSE;
            if (!init_table[i]()) return COREX_FALSE;
        }
    }

    return COREX_TRUE;
}

byte corex_deinit(Corex_Api mask) {
    if (!corex) return COREX_FALSE;
    if (!mask) return COREX_FALSE;

    COREX_FORI(0, 6, 1) {
        if (mask & (1 << i)) {
            if (cleanup_table[i] == NULL) return COREX_FALSE;
            cleanup_table[i]();
            printf("(mask) %d (module)%d", mask, (1 << i));
        }
    }

    return COREX_TRUE;
}

byte corex_cleanup(void) {
    if (!corex) return COREX_TRUE;
    corex_deinit(corex->meta.usage.apis);
    free(corex);
    corex = NULL;
    return COREX_TRUE;
}
