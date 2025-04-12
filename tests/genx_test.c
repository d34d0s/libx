#include <corex/corex.h>

typedef struct Some_Data {
    u32 handle;
    u8 format;
    u8 type;
} Some_Data;

void main() {
    corex_init(COREX_MEMX|COREX_DSX|COREX_GENX);

    Buffer buffer;
    corex->genx.buffer.create((Buffer_Layout){
        .type = BUFFER_TYPE_FLOAT32,
        .access = BUFFER_ACCESS_RW,
        .size = BUFFER_SIZE_8B
    }, &buffer);
    printf("buffer size: %d\n", buffer.meta.size);

    corex->genx.buffer.set_flag(BUFFER_FLAG_AUTOCURSOR, &buffer);
    printf("buffer flags: %d\n", buffer.meta.flags);

    corex->genx.buffer.write(8, &(Some_Data){.handle = 420, .type = 69, .format = 123}, &buffer);
    printf("bytes written: %d (cursor)%d\n", buffer.meta.cursor.written, buffer.meta.cursor.byte);
    
    Some_Data data;
    corex->genx.buffer.read(8, &data, &buffer);
    printf("data read from buffer: %d %d (cursor)%d\n", data.handle, data.format, buffer.meta.cursor.byte);

    corex->genx.buffer.destroy(&buffer);

    if (corex_cleanup()) printf("Genx Test Ran!\n");
}