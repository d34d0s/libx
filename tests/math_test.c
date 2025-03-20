#include "../libx/include/libx_memory.h"
#include "../libx/include/libx_math.h"

int main() {
    libx_init_memory();
    libx_init_math();

    Mat4 s = mathx->mat.scale4(2, 2, 2);
    mathx->mat.print4("s", s);
    
    Mat4 t = mathx->mat.trans4(2, 2, 2);
    mathx->mat.print4("t", t);
    
    Mat4 ts = mathx->mat.mult4(t, s);
    mathx->mat.print4("ts", ts);
    
    Vec3 v = mathx->vec.vec3(1, 2, 3);
    mathx->vec.print3("(none)v", v);
    mathx->vec.print3("(S)v", mathx->mat.mult4v3(s, v));
    mathx->vec.print3("(T)v", mathx->mat.mult4v3(t, v));
    mathx->vec.print3("(TS)v", mathx->mat.mult4v3(ts, v));

    Mat4 view = mathx->mat.lookat(
        mathx->vec.vec3(1, 2, 3),
        mathx->vec.vec3(0, 0, 0),
        mathx->vec.vec3(0, 1, 0)
    );
    mathx->mat.print4("view", view);

    libx_cleanup_math();
    libx_cleanup_memory();
    printf("OK!");
    return 0;
}
