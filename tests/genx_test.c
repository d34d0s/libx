#include <libx/libx.h>

typedef struct Some_Data {
    u32 handle;
    u8 format;
    u8 type;
} Some_Data;

void main() {
    libx_init(LIBX_MEMX|LIBX_DSX|LIBX_GENX);

    Buffer buf;
    libx->genx.buffer.create((Buffer_Layout){
        .type = BUFFER_TYPE_FLOAT32,
        .access = BUFFER_ACCESS_RW,
        .size = BUFFER_SIZE_8B
    }, &buf);
    printf("buffer size: %d\n", libx->genx.buffer.get_field(BUFFER_FIELD_SIZE, &buf));

    libx->genx.buffer.set_flag(BUFFER_FLAG_AUTOCURSOR, &buf);
    printf("buffer flags: %d\n", libx->genx.buffer.get_field(BUFFER_FIELD_FLAGS, &buf));

    libx->genx.buffer.write(8, &(Some_Data){.handle = 420, .type = 69, .format = 123}, &buf);
    printf("bytes written: %d (cursor)%d\n", libx->genx.buffer.get_field(BUFFER_FIELD_WRITTEN, &buf), libx->genx.buffer.get_field(BUFFER_FIELD_BYTE, &buf));
    
    Some_Data data;
    libx->genx.buffer.read(8, &data, &buf);
    printf("data read from buffer: %d %d (cursor)%d\n", data.handle, data.format, libx->genx.buffer.get_field(BUFFER_FIELD_BYTE, &buf));

    libx->genx.buffer.destroy(&buf);

    if (libx_cleanup()) printf("Genx Test Ran!\n");
}