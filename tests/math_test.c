#include "../stdlibx/include/stdx_memory.h"
#include "../stdlibx/include/stdx_math.h"

int main() {
    stdx_init_memory();
    stdx_init_math();

    // VECTOR
    Vector v2 = math_api->create_vector(STDX_VEC2, (f32[]){420.0, 666.0});
    Vector v3 = math_api->create_vector(STDX_VEC3, (f32[]){420.0, 666.0, 123.0});
    Vector v4 = math_api->create_vector(STDX_VEC4, (f32[]){420.0, 666.0, 123.0, 321.0});
    math_api->print_vector("my v2", &v2);
    math_api->print_vector("my v3", &v3);
    math_api->print_vector("my v4", &v4);

    Vector vector = math_api->create_vector(STDX_VEC3, (f32[]){1, 2, 3});
    Vector svector = math_api->scale_vector(5, &vector);
    math_api->print_vector("scaled vector <1, 2, 3> by 5.0", &svector);

    stdx_cleanup_math();
    stdx_cleanup_memory();
    printf("OK!");
    return 0;
}
