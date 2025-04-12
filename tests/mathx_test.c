#include <corex/corex.h>

void main() {
    corex_init(COREX_MATHX);

    Mat4 s = corex->mathx.mat.scale4(2, 2, 2);
    corex->mathx.mat.print4("s", s);
    
    Mat4 t = corex->mathx.mat.trans4(2, 2, 2);
    corex->mathx.mat.print4("t", t);
    
    Mat4 st = corex->mathx.mat.mult4(s, t);
    corex->mathx.mat.print4("st", st);
    
    Vec3 v = corex->mathx.vec.vec3(1, 2, 3);
    corex->mathx.vec.print3("(none)v", v);
    corex->mathx.vec.print3("(S)v", corex->mathx.mat.mult4v3(s, v));
    corex->mathx.vec.print3("(T)v", corex->mathx.mat.mult4v3(t, v));
    corex->mathx.vec.print3("(TS)v", corex->mathx.mat.mult4v3(st, v));

    Mat4 view = corex->mathx.mat.lookat(
        corex->mathx.vec.vec3(1, 2, 3),
        corex->mathx.vec.vec3(0, 0,-1),
        corex->mathx.vec.vec3(0, 1, 0)
    );
    corex->mathx.mat.print4("view", view);

    if (corex_cleanup()) printf("Mathx Test Ran!\n");
}
