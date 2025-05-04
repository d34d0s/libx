#include <corex/corex.h>

typedef struct Some_Data {
    u32 handle;
    u8 format;
    u8 type;
} Some_Data;

void main() {
    corex_init(COREX_MEMX|COREX_DSX|COREX_GENX);
    printf("OK!");
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
    printf("bytes written: %d\n", buffer.meta.cursor.written);
    printf("(cursor) %d\n", corex->genx.buffer.tell(BUFFER_CURSOR_BYTE, &buffer));
    
    Some_Data data;
    corex->genx.buffer.rewind(BUFFER_CURSOR_BYTE, &buffer);
    corex->genx.buffer.read(8, &data, &buffer);
    printf("data read from buffer: %d %d %d\n", data.handle, data.type, data.format);
    printf("(cursor) %d\n", corex->genx.buffer.tell(BUFFER_CURSOR_BYTE, &buffer));

    corex->genx.buffer.seek(BUFFER_CURSOR_BYTE, 2, &buffer);
    printf("(cursor) %d\n", corex->genx.buffer.tell(BUFFER_CURSOR_BYTE, &buffer));
    corex->genx.buffer.seekr(BUFFER_CURSOR_BYTE, -2, &buffer);
    printf("(cursor) %d\n", corex->genx.buffer.tell(BUFFER_CURSOR_BYTE, &buffer));

    if (corex->genx.buffer.reset(&buffer)) printf("buffer reset\n");
    printf("(cursor) %d\n", corex->genx.buffer.tell(BUFFER_CURSOR_BYTE, &buffer));
    
    corex->genx.buffer.seek(BUFFER_CURSOR_BYTE, 7, &buffer);
    printf("(cursor) %d\n", corex->genx.buffer.tell(BUFFER_CURSOR_BYTE, &buffer));
    
    corex->genx.buffer.inject(-4, 4, &(u32){666}, &buffer);
    printf("(cursor) %d\n", corex->genx.buffer.tell(BUFFER_CURSOR_BYTE, &buffer));
    
    u32 more_data;
    corex->genx.buffer.peek(-4, 4, &more_data, &buffer);
    printf("(injected)more data: %d\n", more_data);

    corex->genx.buffer.destroy(&buffer);

    if (corex_cleanup()) printf("Genx Test Ran!\n");
}