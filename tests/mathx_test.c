#include <libx/libx.h>

void main() {
    libx_init(LIBX_MATHX);

    Mat4 s = libx->mathx.mat.scale4(2, 2, 2);
    libx->mathx.mat.print4("s", s);
    
    Mat4 t = libx->mathx.mat.trans4(2, 2, 2);
    libx->mathx.mat.print4("t", t);
    
    Mat4 st = libx->mathx.mat.mult4(s, t);
    libx->mathx.mat.print4("st", st);
    
    Vec3 v = libx->mathx.vec.vec3(1, 2, 3);
    libx->mathx.vec.print3("(none)v", v);
    libx->mathx.vec.print3("(S)v", libx->mathx.mat.mult4v3(s, v));
    libx->mathx.vec.print3("(T)v", libx->mathx.mat.mult4v3(t, v));
    libx->mathx.vec.print3("(TS)v", libx->mathx.mat.mult4v3(st, v));

    Mat4 view = libx->mathx.mat.lookat(
        libx->mathx.vec.vec3(1, 2, 3),
        libx->mathx.vec.vec3(0, 0,-1),
        libx->mathx.vec.vec3(0, 1, 0)
    );
    libx->mathx.mat.print4("view", view);

    if (libx_cleanup()) printf("Mathx Test Ran!\n");
}
