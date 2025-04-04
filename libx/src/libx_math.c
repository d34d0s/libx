#include "../include/libx_math.h"
#include "../include/libx_memory.h"

_libx_math_api* mathx = NULL;


/* ---------------- UTILITY ------------- */
void _print_vec2_impl(cstr name, Vec2 vec) {
    printf("%s: <%0.1f, %0.1f>\n", name,
        vec.x, vec.y);
}

void _print_vec3_impl(cstr name, Vec3 vec) {
    printf("%s: <%0.1f, %0.1f, %0.1f>\n", name,
        vec.x, vec.y, vec.z);
}

void _print_vec4_impl(cstr name, Vec4 vec) {
    printf("%s: <%0.1f, %0.1f, %0.1f, %0.1f>\n", name,
        vec.x, vec.y, vec.z, vec.w);
}

void _print_mat2_impl(cstr name, Mat2 mat) {
    printf("%s\n", name);
    u32 r = 0;
    LIBX_FORI(0, 4, 1) {
        printf("%0.3f ", mat.m[i]);
        if (r++ == 1) {
            printf("\n");
            r = 0;
        }
    }
}

void _print_mat3_impl(cstr name, Mat3 mat) {
    printf("%s\n", name);
    u32 r = 0;
    LIBX_FORI(0, 9, 1) {
        printf("%0.3f ", mat.m[i]);
        if (r++ == 2) {
            printf("\n");
            r = 0;
        }
    }
}

void _print_mat4_impl(cstr name, Mat4 mat) {
    printf("%s\n", name);
    u32 r = 0;
    LIBX_FORI(0, 16, 1) {
        printf("%0.3f ", mat.m[i]);
        if (r++ == 3) {
            printf("\n");
            r = 0;
        }
    }
}
/* ---------------- UTILITY ------------- */


/* ---------------- SCALAR -------------- */
f32 _to_radians_impl(f32 deg) {
    return deg * LIBX_PI/180.0;
}
/* ---------------- SCALAR -------------- */


/* ---------------- VEC2 ---------------- */
// Creates a Vec2
Vec2 _new_vec2_impl(f32 x, f32 y) {
    return (Vec2){x, y};
}

// Scales a Vec2 by a scalar value
Vec2 _scale_vec2_impl(Vec2 v, f32 scale) {
    return (Vec2){v.x * scale, v.y * scale};
}

// Adds two Vec2s
Vec2 _add_vec2_impl(Vec2 v1, Vec2 v2) {
    return (Vec2){v1.x + v2.x, v1.y + v2.y};
}

// Subtracts one Vec2 from another
Vec2 _sub_vec2_impl(Vec2 v1, Vec2 v2) {
    return (Vec2){v1.x - v2.x, v1.y - v2.y};
}

// Dot product of two Vec2s
f32 _dot_vec2_impl(Vec2 v1, Vec2 v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

// Return the magnitude of a Vec2
f32 _mag_vec2_impl(Vec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

// Normalize a Vec2
Vec2 _norm_vec2_impl(Vec2 v) {
    f32 length = mathx->vec.mag2(v);
    return mathx->vec.scale2(v, 1.0f / length);
}
/* ---------------- VEC2 ---------------- */


/* ---------------- VEC3 ---------------- */
// Creates a Vec3
Vec3 _new_vec3_impl(f32 x, f32 y, f32 z) {
    return (Vec3){x, y, z};
}
// Scales a vec by a scalar value
Vec3 _scale_vec3_impl(Vec3 v, f32 scale) {
    Vec3 result;
    result.x = v.x * scale;
    result.y = v.y * scale;
    result.z = v.z * scale;
    return result;
}

// Adds two vectors
Vec3 _add_vec3_impl(Vec3 v1, Vec3 v2) {
    Vec3 result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}

// Subtracts one vec from another
Vec3 _sub_vec3_impl(Vec3 v1, Vec3 v2) {
    Vec3 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

f32 _dot_vec3_impl(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Return the magnitude of a Vec3
f32 _mag_vec3_impl(Vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Normalize a vec
Vec3 _norm_vec3_impl(Vec3 v) {
    f32 length = mathx->vec.mag3(v);
    return mathx->vec.scale3(v, 1.0f / length);
}

// Cross product of two vectors
Vec3 _cross_vec3_impl(Vec3 v1, Vec3 v2) {
    Vec3 result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}
/* ---------------- VEC3 ---------------- */


/* ---------------- VEC4 ---------------- */
// Creates a Vec4
Vec4 _new_vec4_impl(f32 x, f32 y, f32 z, f32 w) {
    return (Vec4){x, y, z, w};
}

// Scales a Vec4 by a scalar value
Vec4 _scale_vec4_impl(Vec4 v, f32 scale) {
    return (Vec4){v.x * scale, v.y * scale, v.z * scale, v.w * scale};
}

// Adds two Vec4s
Vec4 _add_vec4_impl(Vec4 v1, Vec4 v2) {
    return (Vec4){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};
}

// Subtracts one Vec4 from another
Vec4 _sub_vec4_impl(Vec4 v1, Vec4 v2) {
    return (Vec4){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};
}

// Dot product of two Vec4s
f32 _dot_vec4_impl(Vec4 v1, Vec4 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

// Return the magnitude of a Vec4
f32 _mag_vec4_impl(Vec4 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

// Normalize a Vec4
Vec4 _norm_vec4_impl(Vec4 v) {
    f32 length = mathx->vec.mag4(v);
    return mathx->vec.scale4(v, 1.0f / length);
}
/* ---------------- VEC4 ---------------- */


/* ---------------- MAT2 ---------------- */
Mat2 _identity2_impl(void) {
    return (Mat2){.m={
        1, 0,
        0, 1
    }};
}

Mat2 _transpose2_impl(Mat2 mat) {
    Mat2 result = mat;
    LIBX_SWAP(f32, result.m[1], result.m[2]);   // (0,1) <-> (1,0)
    return result;
}

Mat2 _mul_mat2_impl(Mat2 a, Mat2 b) {
    Mat2 result = {0};
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 2; ++col) {
            for (int i = 0; i < 2; ++i) {
                result.m[row * 2 + col] += a.m[row * 2 + i] * b.m[i * 2 + col];
            }
        }
    }
    return result;
}
/* ---------------- MAT2 ---------------- */


/* ---------------- MAT3 ---------------- */
Mat3 _identity3_impl(void) {
    return (Mat3){.m={
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    }};
}

Mat3 _transpose3_impl(Mat3 mat) {
    Mat3 result = mat;
    LIBX_SWAP(f32, result.m[1], result.m[3]);   // (0,1) <-> (1,0)
    LIBX_SWAP(f32, result.m[2], result.m[6]);   // (0,2) <-> (2,0)
    LIBX_SWAP(f32, result.m[5], result.m[7]);   // (1,2) <-> (2,1)
    return result;
}

Mat3 _mul_mat3_impl(Mat3 a, Mat3 b) {
    Mat3 result = {0};
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            for (int i = 0; i < 3; ++i) {
                result.m[row * 3 + col] += a.m[row * 3 + i] * b.m[i * 3 + col];
            }
        }
    }
    return result;
}
/* ---------------- MAT3 ---------------- */


/* ---------------- MAT4 ---------------- */
Mat4 _identity4_impl(void) {
    return (Mat4){.m={
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    }};
}

Mat4 _transpose4_impl(Mat4 mat) {
    Mat4 result = mat;
    LIBX_SWAP(f32, result.m[1], result.m[4]);   // (0,1) <-> (1,0)
    LIBX_SWAP(f32, result.m[2], result.m[8]);   // (0,2) <-> (2,0)
    LIBX_SWAP(f32, result.m[3], result.m[12]);  // (0,3) <-> (3,0)
    LIBX_SWAP(f32, result.m[6], result.m[9]);   // (1,2) <-> (2,1)
    LIBX_SWAP(f32, result.m[7], result.m[13]);  // (1,3) <-> (3,1)
    LIBX_SWAP(f32, result.m[11],result.m[14]);  // (2,3) <-> (3,2)
    return result;
}

Vec3 _mult4v3_impl(Mat4 m, Vec3 v) {
    Vec3 result;
    result.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12];
    result.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13];
    result.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14];
    return result;
}

Mat4 _scale4_impl(f32 x, f32 y, f32 z) {
    Mat4 result = {
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    };
    return result;
}

Mat4 _trans4_impl(f32 x, f32 y, f32 z) {
    Mat4 result = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    };
    return result;
}

Mat4 _rotx4_impl(f32 angle) {
    f32 rad = mathx->scalar.radians(angle);
    f32 cos_angle = cosf(rad);
    f32 sin_angle = sin(rad);
    Mat4 result = {
        1, 0, 0, 0,
        0, cos_angle, sin_angle, 0,
        0,-sin_angle, cos_angle, 0,
        0, 0, 0, 1
    };
    return result;
}

Mat4 _roty4_impl(f32 angle) {
    f32 rad = mathx->scalar.radians(angle);
    f32 cos_angle = cosf(rad);
    f32 sin_angle = sin(rad);
    Mat4 result = {
        cos_angle, 0,-sin_angle, 0,
        0, 1, 0, 0,
        sin_angle, 0, cos_angle, 0,
        0, 0, 0, 1
    };
    return result;
}

Mat4 _rotz4_impl(f32 angle) {
    f32 rad = mathx->scalar.radians(angle);
    f32 cos_angle = cosf(rad);
    f32 sin_angle = sin(rad);
    Mat4 result = {
        cos_angle, sin_angle, 0, 0,
       -sin_angle, cos_angle, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return result;
}

Mat4 _rot4_impl(Vec3 axis, f32 angle) {
    Mat4 result = mathx->mat.identity4();
    f32 rad = mathx->scalar.radians(angle);
    f32 cos_theta = cosf(rad);
    f32 sin_theta = sinf(rad);

    f32 axis_len = mathx->vec.mag3(axis);
    if (axis_len > 0.0f) {
        axis.x /= axis_len;
        axis.y /= axis_len;
        axis.z /= axis_len;
    }

    result.m[0] = cos_theta + (1 - cos_theta) * axis.x * axis.x;
    result.m[1] = (1 - cos_theta) * axis.x * axis.y + sin_theta * axis.z;
    result.m[2] = (1 - cos_theta) * axis.x * axis.z - sin_theta * axis.y;

    result.m[4] = (1 - cos_theta) * axis.y * axis.x - sin_theta * axis.z;
    result.m[5] = cos_theta + (1 - cos_theta) * axis.y * axis.y;
    result.m[6] = (1 - cos_theta) * axis.y * axis.z + sin_theta * axis.x;

    result.m[8] = (1 - cos_theta) * axis.z * axis.x + sin_theta * axis.y;
    result.m[9] = (1 - cos_theta) * axis.z * axis.y - sin_theta * axis.x;
    result.m[10] = cos_theta + (1 - cos_theta) * axis.z * axis.z;

    return result;
}


Mat4 _mult4_impl(Mat4 a, Mat4 b) {
    Mat4 result = {0};
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            for (int i = 0; i < 4; ++i) {
                result.m[row * 4 + col] += a.m[row * 4 + i] * b.m[i * 4 + col];
            }
        }
    }
    return result;
}

Mat4 _lookat_impl(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 target = mathx->vec.add3(eye, center);
    
    Vec3 f = mathx->vec.norm3(mathx->vec.sub3(target, eye)); 
    Vec3 s = mathx->vec.norm3(mathx->vec.cross3(f, up));
    Vec3 u = mathx->vec.norm3(mathx->vec.cross3(s, f));

    Mat4 result = {
        s.x, u.x,-f.x, 0.0f,
        s.y, u.y,-f.y, 0.0f,
        s.z, u.z,-f.z, 0.0f,
       -mathx->vec.dot3(s, eye), -mathx->vec.dot3(u, eye), mathx->vec.dot3(f, eye), 1.0f,
    };
    
    return result;
}

Mat4 _ortho_impl(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
    f32 rml = right - left;
    f32 tmb = top - bottom;
    f32 fmn = far - near;
    
    f32 rpl = right + left;
    f32 tpb = top + bottom;
    f32 fpn = far + near;
    
    Mat4 result = {
        2.0f / rml,  0.0f,       0.0f,       0.0f,
        0.0f,        2.0f / tmb, 0.0f,       0.0f,
        0.0f,        0,         -2.0f / fmn, 0.0f,
        -rpl / rml, -tpb / tmb, -fpn / fmn, 1.0f

    };

    return result;
}

Mat4 _perspective_impl(f32 fovy, f32 aspect, f32 near, f32 far) {
    f32 tfov = tanf(mathx->scalar.radians(fovy) / 2.0f);
    f32 fpn = far + near;
    f32 fmn = far - near;
    f32 fn = far * near;
    
    Mat4 result = {
        1.0f / (aspect * tfov),   0,                0,                       0,
        0,                        1.0f / tfov,      0,                       0,
        0,                        0,               -fpn / fmn,              -1.0f,
        0,                        0,               -(2.0f * fn) / fmn,       1.0f
    };

    return result;
}
/* ---------------- MAT4 ---------------- */

u8 libx_init_math(void) {
    if (mathx != NULL) return LIBX_TRUE;    // redundant call: math API already initialized!

    if (!memx) {
        printf("libx memory api not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize memory api!
    }

    mathx = memx->alloc(sizeof(_libx_math_api), 8);
    if (!mathx) return LIBX_FALSE;   // error: out of memory!

    // SCALAR API INIT
    mathx->scalar.radians = _to_radians_impl;

    // VECTOR API INIT
    mathx->vec.print2 = _print_vec2_impl;
    mathx->vec.print3 = _print_vec3_impl;
    mathx->vec.print4 = _print_vec4_impl;
    
    mathx->vec.vec2 = _new_vec2_impl;
    mathx->vec.add2 = _add_vec2_impl;
    mathx->vec.sub2 = _sub_vec2_impl;
    mathx->vec.dot2 = _dot_vec2_impl;
    mathx->vec.norm2 = _norm_vec2_impl;
    mathx->vec.mag2 = _mag_vec2_impl;
    mathx->vec.scale2 = _scale_vec2_impl;
    
    mathx->vec.vec3 = _new_vec3_impl;
    mathx->vec.add3 = _add_vec3_impl;
    mathx->vec.sub3 = _sub_vec3_impl;
    mathx->vec.dot3 = _dot_vec3_impl;
    mathx->vec.norm3 = _norm_vec3_impl;
    mathx->vec.mag3 = _mag_vec3_impl;
    mathx->vec.scale3 = _scale_vec3_impl;
    mathx->vec.cross3 = _cross_vec3_impl;
    
    mathx->vec.vec4 = _new_vec4_impl;
    mathx->vec.add4 = _add_vec4_impl;
    mathx->vec.sub4 = _sub_vec4_impl;
    mathx->vec.dot4 = _dot_vec4_impl;
    mathx->vec.norm4 = _norm_vec4_impl;
    mathx->vec.mag4 = _mag_vec4_impl;
    mathx->vec.scale4 = _scale_vec4_impl;
    
    mathx->mat.print2 = _print_mat2_impl;
    mathx->mat.print3 = _print_mat3_impl;
    mathx->mat.print4 = _print_mat4_impl;

    // MATRIX 2
    mathx->mat.identity2 = _identity2_impl;
    mathx->mat.transpose2 = _transpose2_impl;
    
    mathx->mat.mult2 = _mul_mat2_impl;
    
    // MATRIX 3
    mathx->mat.identity3 = _identity3_impl;
    mathx->mat.transpose3 = _transpose3_impl;
    
    mathx->mat.mult3 = _mul_mat3_impl;

    // MATRIX 4
    mathx->mat.identity4 = _identity4_impl;
    mathx->mat.transpose4 = _transpose4_impl;
    
    mathx->mat.rot4 = _rot4_impl;
    mathx->mat.rotx4 = _rotx4_impl;
    mathx->mat.roty4 = _roty4_impl;
    mathx->mat.rotz4 = _rotz4_impl;
    mathx->mat.trans4 = _trans4_impl;
    mathx->mat.scale4 = _scale4_impl;
    
    mathx->mat.mult4 = _mult4_impl;
    mathx->mat.mult4v3 = _mult4v3_impl;
    
    mathx->mat.lookat = _lookat_impl;
    
    mathx->mat.ortho = _ortho_impl;
    mathx->mat.perspective = _perspective_impl;

    return LIBX_TRUE;
}

void libx_cleanup_math(void) {
    if (mathx == NULL) return;    // error: math API not initialized!
    memx->dealloc(mathx);
    mathx = NULL;
}

