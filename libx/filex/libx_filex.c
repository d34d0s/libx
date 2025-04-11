#include <libx/libx.h>

u8 _exists_impl(cstr path) {
    FILE* file = fopen(path, "rb");
    if (!file) return LIBX_FALSE;
    fclose(file);
    return LIBX_TRUE;
}

u8 _copy_impl(cstr src, cstr dest, u64 size) {
    FILE* srcf = fopen(src, "rb");
    if (!srcf) return LIBX_FALSE;   // error: file not found!
    
    FILE* destf = fopen(dest, "wb");
    if (!destf) {
        fclose(srcf);
        return LIBX_FALSE;  // error: file not found!
    }

    u64 read = 0;
    char buffer[size];
    do fwrite(buffer, sizeof(char), read, destf);
    while((read = fread(buffer, sizeof(char), size, srcf)) > 0);
    
    fclose(srcf);
    fclose(destf);
    return LIBX_TRUE;
}

u64 _size_impl(cstr path) {
    FILE* file = fopen(path, "rb");
    if (!file) return 0;    // error: file not found!
    
    fseek(file, 0, SEEK_END);
    u64 read = ftell(file);
    fclose(file);

    return read;
}

str _read_impl(cstr path, u64 size) {
    FILE* file = fopen(path, "rb");
    if (!file) return NULL; // error: file not found!

    if (!size) size = libx->filex.size(path);

    str out = libx->memx.alloc(size+1, 8);
    if (!out) return NULL;  // error: out of memory!
    
    u64 read = 0;
    read = fread(out, sizeof(char), size, file);
    if (read != size) {
        libx->memx.dealloc(out);
        fclose(file);
        return NULL;    // error: failed to read file!
    }
    
    out[size] = '\0';
    fclose(file);
    return out;
}

u8 _readb_impl(cstr path, u64 size, void* out) {
    FILE* file = fopen(path, "rb");
    if (!file) return LIBX_FALSE; // error: file not found!

    u64 read = 0;
    read = fread(out, size, 1, file);
    if (read != size) {
        fclose(file);
        return LIBX_FALSE;    // error: failed to fully read data!
    }
    
    fclose(file);
    return LIBX_TRUE;
}

u8 _remove_impl(cstr path) {
    if (remove(path) != LIBX_FALSE) return LIBX_FALSE;  // error: failed to delete file!
    return LIBX_TRUE;
}

u8 _write_impl(cstr path, cstr lines, u8 preserve) {
    FILE* file = fopen(path, preserve ? "ab" : "wb");
    if (!file) return LIBX_FALSE;   // error: failed to write to file!

    u64 size = strlen(lines);
    if (!size) {
        fclose(file);
        return LIBX_FALSE;  // error: failed to get size of `lines`!
    }

    u64 written = fwrite(lines, sizeof(char), size, file);
    if (written != size) {
        fclose(file);
        return LIBX_FALSE;  // error: failed to write all lines!
    }
    
    fclose(file);
    return LIBX_TRUE;
}

u8 _writeb_impl(cstr path, u64 size, u8 preserve, void* ptr) {
    FILE* file = fopen(path, preserve ? "ab" : "wb");
    if (!file) return LIBX_FALSE;   // error: failed to write to file!

    u64 written = fwrite(ptr, size, 1, file);
    if (written != size) {
        fclose(file);
        return LIBX_FALSE;  // error: failed to write all lines!
    }
    
    fclose(file);
    return LIBX_TRUE;
}

u8 _append_impl(cstr path, cstr lines, u8 newline) {
    u8 result = libx->filex.write(path, lines, 1);
    if (newline) result = libx->filex.write(path, "\n", 1);
    return result;
}

u8 _appendb_impl(cstr path, u64 size, u8 newline, void* ptr) {
    u8 result = libx->filex.writeb(path, size, 1, ptr);
    if (newline) result = libx->filex.write(path, "\n", 1);
    return result;
}

u8 _process_impl(cstr path, u64 lines, void (*callback)(cstr line)) {
    FILE* file = fopen(path, "rb");
    if (!file) return LIBX_FALSE;    // error: failed to open file!

    str buffer = libx->memx.alloc(lines, 8);
    if (!buffer) return LIBX_FALSE; // error: failed to allocate temporary buffer!
    
    do callback((cstr)buffer);
    while(fgets(buffer, lines ,file));
    
    libx->memx.dealloc(buffer);
    fclose(file);

    return LIBX_TRUE;
}


u8 _libx_init_filex(void) {
    if (!libx) return LIBX_FALSE; // error: null ptr!
    if (libx->filex.init == LIBX_TRUE) return LIBX_TRUE;    // redundant call: Filex API already initialized!

    if (libx->memx.init == LIBX_FALSE) {
        printf("libx Memx api not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize Memx api!
    }
    
    libx->filex.exists = _exists_impl;
    libx->filex.copy = _copy_impl;
    libx->filex.size = _size_impl;
    libx->filex.read = _read_impl;
    libx->filex.readb = _readb_impl;
    libx->filex.write = _write_impl;
    libx->filex.remove = _remove_impl;
    libx->filex.append = _append_impl;
    libx->filex.writeb = _writeb_impl;
    libx->filex.appendb = _appendb_impl;
    libx->filex.process = _process_impl;

	libx->filex.init = LIBX_TRUE;
    return LIBX_TRUE;
}

void _libx_cleanup_filex(void) {
    if (libx->filex.init == LIBX_FALSE) return;    // error: Filex API not initialized!
	libx->meta.usage.apis &= ~LIBX_FILEX;
	libx->filex.init = LIBX_FALSE;
	libx->filex	= (Filex){0};
}
