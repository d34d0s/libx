#include "../libx/include/libx_fio.h"
#include "../libx/include/libx_memory.h"

void file_fn(cstr line) {
    printf("processing line: %s\n", line);
}

typedef struct some_struct {
    u32 n1;
    u32 n2;
} some_struct;

int main() {
    libx_init_memory();
    libx_init_fileio();

    cstr test_file = "../tests/test_file.bin";
    cstr test_file2 = "../tests/test_file2.bin";
    
    if (fileio_api->exists(test_file)) printf("test file exists!\n");
    if (fileio_api->exists(test_file2)) printf("test file2 exists!\n");
    
    // write some binary data
    some_struct my_struct = {123, 321};
    if (fileio_api->writeb(test_file, sizeof(some_struct), 0, &my_struct))
        printf("wrote to test file!\n");
    
    some_struct my_struct2 = {420, 69};
    if (fileio_api->appendb(test_file, sizeof(some_struct), 0, &my_struct2))
        printf("appended to test file!\n");
    
    u64 dummy_size = fileio_api->size(test_file);
    if (dummy_size > 0) printf("test file size: %llu\n", dummy_size);
    
    some_struct out_structs[2];
    fileio_api->readb(test_file, sizeof(some_struct) * 2, &out_structs);
    LIBX_FORI(0, 2, 1) printf("test data: %d %d\n", out_structs[i].n1, out_structs[i].n2);
    
    if (fileio_api->copy(test_file, test_file2, dummy_size)) {
        printf("copied test file!\n");
    
        some_struct out_structs2[2];
        fileio_api->readb(test_file2, sizeof(some_struct) * 2, &out_structs2);
        LIBX_FORI(0, 2, 1) printf("test data2: %d %d\n", out_structs2[i].n1, out_structs2[i].n2);
    }
        
    if (fileio_api->remove("delete.txt"))
        printf("deleted 'delete.txt'!\n");
    
    if (fileio_api->process(test_file, dummy_size, file_fn))
        printf("processed test file!\n");

    libx_cleanup_fileio();
    libx_cleanup_memory();
    return 0;
}
