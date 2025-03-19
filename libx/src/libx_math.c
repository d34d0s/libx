#include "../include/libx_math.h"
#include "../include/libx_memory.h"

_libx_math_api* math_api = NULL;


/* ---------------- SCALAR ---------------- */
f32 _to_radians_impl(f32 deg) {
    return deg * LIBX_PI/180.0;
}
/* ---------------- SCALAR ---------------- */


/* ---------------- VECTOR ---------------- */
void _print_vec2_impl(cstr name, Vec2 vector) {
    printf("%s: <%0.1f, %0.1f>\n", name,
        vector.x, vector.y);
}

void _print_vec3_impl(cstr name, Vec3 vector) {
    printf("%s: <%0.1f, %0.1f, %0.1f>\n", name,
        vector.x, vector.y, vector.z);
}

void _print_vec4_impl(cstr name, Vec4 vector) {
    printf("%s: <%0.1f, %0.1f, %0.1f, %0.1f>\n", name,
        vector.x, vector.y, vector.z, vector.w);
}

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

// Normalize a Vec2 (make it unit length)
Vec2 _norm_vec2_impl(Vec2 v) {
    f32 length = sqrtf(v.x * v.x + v.y * v.y);
    return math_api->scale_vec2(v, 1.0f / length);
}

// Creates a Vec3
Vec3 _new_vec3_impl(f32 x, f32 y, f32 z) {
    return (Vec3){x, y, z};
}
// Scales a vector by a scalar value
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

// Subtracts one vector from another
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

// Normalize a vector (make it unit length)
Vec3 _norm_vec3_impl(Vec3 v) {
    f32 length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return math_api->scale_vec3(v, 1.0f / length);
}

// Cross product of two vectors
Vec3 _cross_vec3_impl(Vec3 v1, Vec3 v2) {
    Vec3 result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}


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

// Normalize a Vec4 (make it unit length)
Vec4 _norm_vec4_impl(Vec4 v) {
    f32 length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
    return math_api->scale_vec4(v, 1.0f / length);
}
/* ---------------- VECTOR ---------------- */

/* ---------------- MATRIX 4 ---------------- */
void _print_mat4_impl(cstr name, Mat4 matrix) {
    printf("%s\n", name);
    u32 r = 0;
    LIBX_FORI(0, 16, 1) {
        printf("%0.3f ", matrix.m[i]);
        if (r++ == 3) {
            printf("\n");
            r = 0;
        }
    }
}

Mat4 _identity_impl(void) {
    Mat4 result = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return result;
}

Mat4 _transpose_impl(Mat4 mat) {
    Mat4 result = mat; // Copy original matrix
    // Swap elements across the diagonal
    #define SWAP(a, b) { f32 temp = result.m[a]; result.m[a] = result.m[b]; result.m[b] = temp; }
    
    SWAP(1, 4);   // (0,1) <-> (1,0)
    SWAP(2, 8);   // (0,2) <-> (2,0)
    SWAP(3, 12);  // (0,3) <-> (3,0)
    SWAP(6, 9);   // (1,2) <-> (2,1)
    SWAP(7, 13);  // (1,3) <-> (3,1)
    SWAP(11, 14); // (2,3) <-> (3,2)
    
    return result;
}

Vec3 _mul_mat4_vec3_impl(Mat4 m, Vec3 v) {
    Vec3 result;
    result.x = m.m[0] * v.x + m.m[1] * v.y + m.m[2] * v.z + m.m[3];
    result.y = m.m[4] * v.x + m.m[5] * v.y + m.m[6] * v.z + m.m[7];
    result.z = m.m[8] * v.x + m.m[9] * v.y + m.m[10] * v.z + m.m[11];
    return result;
}

Mat4 _trans_mat4_impl(f32 x, f32 y, f32 z) {
    Mat4 result = {
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
    };
    return result;
}

Mat4 _scale_mat4_impl(f32 x, f32 y, f32 z) {
    Mat4 result = {
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    };
    return result;
}

// arbitrary 3D axis rotation
Mat4 _rot_mat4_impl(f32 x, f32 y, f32 z, f32 angle) {
    Mat4 result = math_api->identity();
    f32 rad = math_api->to_radians(angle);
    f32 cos_theta = cosf(rad);
    f32 sin_theta = sinf(rad);

    f32 axis_len = sqrtf(x * x + y * y + z * z);
    if (axis_len > 0.0f) {
        x /= axis_len;
        y /= axis_len;
        z /= axis_len;
    }

    result.m[0] = cos_theta + (1 - cos_theta) * x * x;
    result.m[1] = (1 - cos_theta) * x * y + sin_theta * z;
    result.m[2] = (1 - cos_theta) * x * z - sin_theta * y;

    result.m[4] = (1 - cos_theta) * y * x - sin_theta * z;
    result.m[5] = cos_theta + (1 - cos_theta) * y * y;
    result.m[6] = (1 - cos_theta) * y * z + sin_theta * x;

    result.m[8] = (1 - cos_theta) * z * x + sin_theta * y;
    result.m[9] = (1 - cos_theta) * z * y - sin_theta * x;
    result.m[10] = cos_theta + (1 - cos_theta) * z * z;

    return result;
}

Mat4 _rotx_mat4_impl(f32 angle) {
    f32 rad = math_api->to_radians(angle);
    f32 cos_angle = cosf(rad);
    f32 sin_angle = sin(rad);
    Mat4 result = {
        1.0, 0, 0, 0,
        0, cos_angle, sin_angle, 0,
        0, -sin_angle, cos_angle, 0,
        0, 0, 0, 1
    };
    return result;
}

Mat4 _roty_mat4_impl(f32 angle) {
    f32 rad = math_api->to_radians(angle);
    f32 cos_angle = cosf(rad);
    f32 sin_angle = sin(rad);
    Mat4 result = {
        cos_angle, 0, -sin_angle, 0,
        0, 1.0, 0, 0,
        sin_angle, 0, cos_angle, 0,
        0, 0, 0, 1
    };
    return result;
}

Mat4 _rotz_mat4_impl(f32 angle) {
    f32 rad = math_api->to_radians(angle);
    f32 cos_angle = cosf(rad);
    f32 sin_angle = sin(rad);
    Mat4 result = {
        cos_angle, sin_angle, 0, 0,
       -sin_angle, cos_angle, 0, 0,
        0, 0, 1.0, 0,
        0, 0, 0, 1.0
    };
    return result;
}

Mat4 _mul_mat4_impl(Mat4 a, Mat4 b) {
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

Mat4 _ortho_impl(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
    f32 rml = right - left;   // right - left
    f32 tmb = top - bottom;   // top - bottom
    f32 fmn = far - near;     // far - near
    
    Mat4 result = {
        2.0f / rml,  0,          0,          0,
        0,           2.0f / tmb, 0,          0,
        0,           0,         -2.0f / fmn, 0,
       -(right + left) / rml, -(top + bottom) / tmb, -(far + near) / fmn, 1
    };

    return result;
}

Mat4 _perspective_impl(f32 fov, f32 aspect, f32 near, f32 far) {
    f32 tfov = tanf(fov / 2.0f);
    f32 fpn = far + near;
    f32 fmn = far - near;
    f32 fn = far * near;
    
    Mat4 result = {
        1.0f / (aspect * tfov),   0,                0,  0,
        0,                        1.0f / tfov,      0,  0,
        0,                        0,               -fpn / fmn, -(2.0f * fn) / fmn,
        0,                        0,               -1.0f,       1
    };
    
    return result;
}

Mat4 _look_at_impl(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 directionVec = math_api->norm_vec3(math_api->sub_vec3(center, eye));
    Vec3 rightVec = math_api->norm_vec3(math_api->cross_vec3(directionVec, up));
    Vec3 upVec = math_api->cross_vec3(rightVec, directionVec);
    
    Mat4 result = math_api->identity();
    result.m[0] = rightVec.x; result.m[1] = upVec.x; result.m[2] = -directionVec.x;
    result.m[4] = rightVec.y; result.m[5] = upVec.y; result.m[6] = -directionVec.y;
    result.m[8] = rightVec.z; result.m[9] = upVec.z; result.m[10] = -directionVec.z;
    result.m[12] = -math_api->dot_vec3(rightVec, eye); result.m[13] = -math_api->dot_vec3(upVec, eye); result.m[14] = -math_api->dot_vec3(directionVec, eye);
    
    return result;
}
/* ---------------- MATRIX 4 ---------------- */

u8 libx_init_math(void) {
    math_api = memory_api->alloc(sizeof(_libx_math_api), 16);
    if (!math_api) return LIBX_FALSE;   // error: out of memory!
    
    math_api->print_vec2 = _print_vec2_impl;
    math_api->print_vec3 = _print_vec3_impl;
    math_api->print_vec4 = _print_vec4_impl;
    math_api->print_mat4 = _print_mat4_impl;

    math_api->to_radians = _to_radians_impl;
    
    math_api->new_vec2 = _new_vec2_impl;
    math_api->scale_vec2 = _scale_vec2_impl;
    math_api->add_vec2 = _add_vec2_impl;
    math_api->sub_vec2 = _sub_vec2_impl;
    math_api->dot_vec2 = _dot_vec2_impl;
    math_api->norm_vec2 = _norm_vec2_impl;
    
    math_api->new_vec3 = _new_vec3_impl;
    math_api->scale_vec3 = _scale_vec3_impl;
    math_api->add_vec3 = _add_vec3_impl;
    math_api->sub_vec3 = _sub_vec3_impl;
    math_api->dot_vec3 = _dot_vec3_impl;
    math_api->norm_vec3 = _norm_vec3_impl;
    math_api->cross_vec3 = _cross_vec3_impl;
    
    math_api->new_vec4 = _new_vec4_impl;
    math_api->scale_vec4 = _scale_vec4_impl;
    math_api->add_vec4 = _add_vec4_impl;
    math_api->sub_vec4 = _sub_vec4_impl;
    math_api->dot_vec4 = _dot_vec4_impl;
    math_api->norm_vec4 = _norm_vec4_impl;
    
    math_api->identity = _identity_impl;
    math_api->transpose = _transpose_impl;
    
    math_api->rotate = _rot_mat4_impl;
    math_api->rotate_x = _rotx_mat4_impl;
    math_api->rotate_y = _roty_mat4_impl;
    math_api->rotate_z = _rotz_mat4_impl;
    math_api->translate = _trans_mat4_impl;
    math_api->scale_mat4 = _scale_mat4_impl;
    
    math_api->mul_mat4 = _mul_mat4_impl;
    math_api->mul_mat4_vec3 = _mul_mat4_vec3_impl;
    
    math_api->look_at = _look_at_impl;
    
    math_api->ortho = _ortho_impl;
    math_api->perspective = _perspective_impl;

    return LIBX_TRUE;
}

void libx_cleanup_math(void) {
    memory_api->dealloc(math_api);
}

