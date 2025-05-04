#include <corex/corex.h>

byte _exists_impl(cstr path) {
    FILE* file = fopen(path, "rb");
    if (!file) return COREX_FALSE;
    fclose(file);
    return COREX_TRUE;
}

byte _copy_impl(cstr src, cstr dest, u64 size) {
    FILE* srcf = fopen(src, "rb");
    if (!srcf) return COREX_FALSE;   // error: file not found!
    
    FILE* destf = fopen(dest, "wb");
    if (!destf) {
        fclose(srcf);
        return COREX_FALSE;  // error: file not found!
    }

    u64 read = 0;
    char buffer[size];
    do fwrite(buffer, sizeof(char), read, destf);
    while((read = fread(buffer, sizeof(char), size, srcf)) > 0);
    
    fclose(srcf);
    fclose(destf);
    return COREX_TRUE;
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

    if (!size) size = corex->filex.size(path);

    str out = corex->memx.alloc(size+1, 8);
    if (!out) return NULL;  // error: out of memory!
    
    u64 read = 0;
    read = fread(out, sizeof(char), size, file);
    if (read != size) {
        corex->memx.dealloc(out);
        fclose(file);
        return NULL;    // error: failed to read file!
    }
    
    out[size] = '\0';
    fclose(file);
    return out;
}

byte _readb_impl(cstr path, u64 size, void* out) {
    FILE* file = fopen(path, "rb");
    if (!file) return COREX_FALSE; // error: file not found!

    u64 read = 0;
    read = fread(out, size, 1, file);
    if (read != size) {
        fclose(file);
        return COREX_FALSE;    // error: failed to fully read data!
    }
    
    fclose(file);
    return COREX_TRUE;
}

byte _remove_impl(cstr path) {
    if (remove(path) != COREX_FALSE) return COREX_FALSE;  // error: failed to delete file!
    return COREX_TRUE;
}

byte _write_impl(cstr path, cstr lines, byte preserve) {
    FILE* file = fopen(path, preserve ? "ab" : "wb");
    if (!file) return COREX_FALSE;   // error: failed to write to file!

    u64 size = strlen(lines);
    if (!size) {
        fclose(file);
        return COREX_FALSE;  // error: failed to get size of `lines`!
    }

    u64 written = fwrite(lines, sizeof(char), size, file);
    if (written != size) {
        fclose(file);
        return COREX_FALSE;  // error: failed to write all lines!
    }
    
    fclose(file);
    return COREX_TRUE;
}

byte _writeb_impl(cstr path, u64 size, byte preserve, void* ptr) {
    FILE* file = fopen(path, preserve ? "ab" : "wb");
    if (!file) return COREX_FALSE;   // error: failed to write to file!

    u64 written = fwrite(ptr, size, 1, file);
    if (written != size) {
        fclose(file);
        return COREX_FALSE;  // error: failed to write all lines!
    }
    
    fclose(file);
    return COREX_TRUE;
}

byte _append_impl(cstr path, cstr lines, byte newline) {
    byte result = corex->filex.write(path, lines, 1);
    if (newline) result = corex->filex.write(path, "\n", 1);
    return result;
}

byte _appendb_impl(cstr path, u64 size, byte newline, void* ptr) {
    byte result = corex->filex.writeb(path, size, 1, ptr);
    if (newline) result = corex->filex.write(path, "\n", 1);
    return result;
}

byte _process_impl(cstr path, u64 lines, void (*callback)(cstr line)) {
    FILE* file = fopen(path, "rb");
    if (!file) return COREX_FALSE;    // error: failed to open file!

    str buffer = corex->memx.alloc(lines, 8);
    if (!buffer) return COREX_FALSE; // error: failed to allocate temporary buffer!
    
    do callback((cstr)buffer);
    while(fgets(buffer, lines ,file));
    
    corex->memx.dealloc(buffer);
    fclose(file);

    return COREX_TRUE;
}


byte _corex_init_filex(void) {
    if (!corex) return COREX_FALSE; // error: null ptr!
    if (corex->filex.init == COREX_TRUE) return COREX_TRUE;    // redundant call: Filex API already initialized!

    if (corex->memx.init == COREX_FALSE) {
        printf("corex Memx api not initialized!\n");
        return COREX_FALSE; // error: failed to initialize Memx api!
    }
    
    corex->filex.exists = _exists_impl;
    corex->filex.copy = _copy_impl;
    corex->filex.size = _size_impl;
    corex->filex.read = _read_impl;
    corex->filex.readb = _readb_impl;
    corex->filex.write = _write_impl;
    corex->filex.remove = _remove_impl;
    corex->filex.append = _append_impl;
    corex->filex.writeb = _writeb_impl;
    corex->filex.appendb = _appendb_impl;
    corex->filex.process = _process_impl;

    corex->meta.usage.filex = sizeof(Filex);
	corex->filex.init = COREX_TRUE;
    return COREX_TRUE;
}

void _corex_cleanup_filex(void) {
    if (corex->filex.init == COREX_FALSE) return;    // error: Filex API not initialized!
	corex->meta.usage.apis &= ~COREX_FILEX;
	corex->filex.init = COREX_FALSE;
    corex->meta.usage.filex = 0;
	corex->filex	= (Filex){0};
}
