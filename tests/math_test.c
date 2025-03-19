#include "../libx/include/libx_memory.h"
#include "../libx/include/libx_math.h"

int main() {
    libx_init_memory();
    libx_init_math();

    Vec3 xbasis = math_api->new_vec3(1, 0, 0);
    math_api->print_vec3("X Basis Vector", xbasis);
    
    Vec3 ybasis = math_api->new_vec3(0, 1, 0);
    math_api->print_vec3("Y Basis Vector", ybasis);
    
    Vec3 xcy = math_api->cross_vec3(ybasis, xbasis);
    math_api->print_vec3("Ybasis cross Xbasis", xcy);
    
    Vec3 v = math_api->new_vec3(1, 2, 3);
    math_api->print_vec3("My Vector", v);
    
    Mat4 s = math_api->scale_mat4(5, 5, 5);
     Mat4 t = math_api->translate(1, 1, 1);
     Mat4 st = math_api->mul_mat4(s, t);
     math_api->print_mat4("Matrix(s)", s);
     math_api->print_mat4("Matrix(t)", t);
     math_api->print_mat4("Matrix(st)", st);
     
    Mat4 Tt = math_api->transpose(t);
    math_api->print_mat4("Matrix(Tt)", Tt);
    
    v = math_api->mul_mat4_vec3(st, v);
    math_api->print_vec3("My Vector(st) | expected (10, 15, 20)", v);
    
    Mat4 u_view = math_api->look_at(
        math_api->new_vec3(0, 0, 3),
        math_api->new_vec3(0, 0, 0),
        math_api->new_vec3(0, 1, 0)
    );
    math_api->print_mat4("Matrix(u_view)", u_view);
    
    Mat4 u_proj = math_api->perspective(45.0, 800/600, 0.1, 1000);
    math_api->print_mat4("Matrix(u_proj)", u_proj);
    
    libx_cleanup_math();
    libx_cleanup_memory();
    printf("OK!");
    return 0;
}
