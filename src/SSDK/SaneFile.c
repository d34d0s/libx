#include <include/SSDK/SSDK.h>

SaneFile* saneFile = NULL;

byte _existsImpl(cstr path) {
    saneFile->module.calls++;
    FILE* file = fopen(path, "rb");
    if (!file) return SSDK_FALSE;
    fclose(file);
    return SSDK_TRUE;
}

byte _copyImpl(cstr src, cstr dest, u64 size) {
    saneFile->module.calls++;
    FILE* srcf = fopen(src, "rb");
    if (!srcf) return SSDK_FALSE;   // error: file not found!
    
    FILE* destf = fopen(dest, "wb");
    if (!destf) {
        fclose(srcf);
        return SSDK_FALSE;  // error: file not found!
    }

    u64 read = 0;
    char buffer[size];
    do fwrite(buffer, sizeof(char), read, destf);
    while((read = fread(buffer, sizeof(char), size, srcf)) > 0);
    
    fclose(srcf);
    fclose(destf);
    return SSDK_TRUE;
}

u64 _sizeImpl(cstr path) {
    saneFile->module.calls++;
    FILE* file = fopen(path, "rb");
    if (!file) return 0;    // error: file not found!
    
    fseek(file, 0, SEEK_END);
    u64 read = ftell(file);
    fclose(file);

    return read;
}

str _readImpl(cstr path, u64 size) {
    saneFile->module.calls++;
    FILE* file = fopen(path, "rb");
    if (!file) return NULL; // error: file not found!

    if (!size) size = saneFile->size(path);

    str out = saneMemory->alloc(size+1, 8);
    if (!out) return NULL;  // error: out of memory!
    
    u64 read = 0;
    read = fread(out, sizeof(char), size, file);
    if (read != size) {
        saneMemory->dealloc(out);
        fclose(file);
        return NULL;    // error: failed to read file!
    }
    
    out[size] = '\0';
    fclose(file);
    return out;
}

byte _readbImpl(cstr path, u64 size, ptr out) {
    saneFile->module.calls++;
    FILE* file = fopen(path, "rb");
    if (!file) return SSDK_FALSE; // error: file not found!

    u64 read = 0;
    read = fread(out, size, 1, file);
    if (read != size) {
        fclose(file);
        return SSDK_FALSE;    // error: failed to fully read data!
    }
    
    fclose(file);
    return SSDK_TRUE;
}

byte _removeImpl(cstr path) {
    saneFile->module.calls++;
    if (remove(path) != SSDK_FALSE) return SSDK_FALSE;  // error: failed to delete file!
    return SSDK_TRUE;
}

byte _writeImpl(cstr path, cstr lines, byte preserve) {
    saneFile->module.calls++;
    FILE* file = fopen(path, preserve ? "ab" : "wb");
    if (!file) return SSDK_FALSE;   // error: failed to write to file!

    u64 size = strlen(lines);
    if (!size) {
        fclose(file);
        return SSDK_FALSE;  // error: failed to get size of `lines`!
    }

    u64 written = fwrite(lines, sizeof(char), size, file);
    if (written != size) {
        fclose(file);
        return SSDK_FALSE;  // error: failed to write all lines!
    }
    
    fclose(file);
    return SSDK_TRUE;
}

byte _writebImpl(cstr path, u64 size, byte preserve, ptr buf) {
    saneFile->module.calls++;
    FILE* file = fopen(path, preserve ? "ab" : "wb");
    if (!file) return SSDK_FALSE;   // error: failed to write to file!

    u64 written = fwrite(buf, size, 1, file);
    if (written != size) {
        fclose(file);
        return SSDK_FALSE;  // error: failed to write all lines!
    }
    
    fclose(file);
    return SSDK_TRUE;
}

byte _appendImpl(cstr path, cstr lines, byte newline) {
    saneFile->module.calls++;
    byte result = saneFile->write(path, lines, 1);
    if (newline) result = saneFile->write(path, "\n", 1);
    return result;
}

byte _appendbImpl(cstr path, u64 size, byte newline, ptr buf) {
    saneFile->module.calls++;
    byte result = saneFile->writeb(path, size, 1, buf);
    if (newline) result = saneFile->write(path, "\n", 1);
    return result;
}

byte _processImpl(cstr path, u64 lines, none (*callback)(cstr line)) {
    saneFile->module.calls++;
    FILE* file = fopen(path, "rb");
    if (!file) return SSDK_FALSE;    // error: failed to open file!

    str buffer = saneMemory->alloc(lines, 8);
    if (!buffer) return SSDK_FALSE; // error: failed to allocate temporary buffer!
    
    do callback((cstr)buffer);
    while(fgets(buffer, lines ,file));
    
    saneMemory->dealloc(buffer);
    fclose(file);

    return SSDK_TRUE;
}


byte ssdkInitFile(none) {
    if (saneFile != NULL) return SSDK_TRUE;

    if (saneMemory == NULL) {
        if (saneLog != NULL) {
            saneLog->log(SANE_LOG_ERROR, "SaneMemory Not Initialized!");
        } else { printf("SaneMemory Not Initialized!\n"); }
        return SSDK_FALSE;
    }

    saneFile = malloc(sizeof(SaneFile));
    if (saneFile == NULL) return SSDK_FALSE;  // error: out of memory!
    
    saneFile->exists = _existsImpl;
    saneFile->copy = _copyImpl;
    saneFile->size = _sizeImpl;
    saneFile->read = _readImpl;
    saneFile->readb = _readbImpl;
    saneFile->write = _writeImpl;
    saneFile->remove = _removeImpl;
    saneFile->append = _appendImpl;
    saneFile->writeb = _writebImpl;
    saneFile->appendb = _appendbImpl;
    saneFile->process = _processImpl;

    saneFile->module.mask = 0;
    saneFile->module.calls = 0;
    saneFile->module.name = "SaneFile";
    saneFile->module.size = sizeof(SaneFile);
    return SSDK_TRUE;
}

none ssdkExitFile(none) {
    if (saneFile == NULL) return;
	free(saneFile);
    saneFile = NULL;
}

