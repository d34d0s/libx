#include "../include/libx_fio.h"
#include "../include/libx_memory.h"

_libx_fileio_api* fileio_api = NULL;

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

    if (!size) size = fileio_api->size(path);

    str out = memory_api->alloc(size+1, 16);
    if (!out) return NULL;  // error: out of memory!
    
    u64 read = 0;
    read = fread(out, sizeof(char), size, file);
    if (read != size) {
        memory_api->dealloc(out);
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
    u8 result = fileio_api->write(path, lines, 1);
    if (newline) result = fileio_api->write(path, "\n", 1);
    return result;
}

u8 _appendb_impl(cstr path, u64 size, u8 newline, void* ptr) {
    u8 result = fileio_api->writeb(path, size, 1, ptr);
    if (newline) result = fileio_api->write(path, "\n", 1);
    return result;
}

u8 _process_impl(cstr path, u64 lines, void (*callback)(cstr line)) {
    FILE* file = fopen(path, "rb");
    if (!file) return LIBX_FALSE;    // error: failed to open file!

    str buffer = memory_api->alloc(lines, 16);
    if (!buffer) return LIBX_FALSE; // error: failed to allocate temporary buffer!
    
    do callback((cstr)buffer);
    while(fgets(buffer, lines ,file));
    
    memory_api->dealloc(buffer);
    fclose(file);

    return LIBX_TRUE;
}


u8 libx_init_fileio(void) {
    fileio_api  = memory_api->alloc(sizeof(_libx_fileio_api), 16);
    if (!fileio_api) return LIBX_FALSE; // error: out of memory!

    fileio_api->exists = _exists_impl;
    fileio_api->copy = _copy_impl;
    fileio_api->size = _size_impl;
    fileio_api->read = _read_impl;
    fileio_api->readb = _readb_impl;
    fileio_api->write = _write_impl;
    fileio_api->remove = _remove_impl;
    fileio_api->append = _append_impl;
    fileio_api->writeb = _writeb_impl;
    fileio_api->appendb = _appendb_impl;
    fileio_api->process = _process_impl;

    return LIBX_TRUE;
}

void libx_cleanup_fileio(void) {
    memory_api->dealloc(fileio_api);
}
