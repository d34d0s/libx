#include <include/SSDK/SSDK.h>
#include <math.h>

SaneMath* saneMath = NULL;

/* ---------------- UTILITY ------------- */
void _logVec2Impl(cstr name, Vec2 vec) {
    ssdkInitLog();
    saneLog->logFmt(SANE_LOG_INFO, "%s: <%0.1f, %0.1f>", name,
        vec.x, vec.y);
}

void _logVec3Impl(cstr name, Vec3 vec) {
    ssdkInitLog();
    saneLog->logFmt(SANE_LOG_INFO, "%s: <%0.1f, %0.1f, %0.1f>", name,
        vec.x, vec.y, vec.z);
}

void _logVec4Impl(cstr name, Vec4 vec) {
    ssdkInitLog();
    saneLog->logFmt(SANE_LOG_INFO, "%s: <%0.1f, %0.1f, %0.1f, %0.1f>", name,
        vec.x, vec.y, vec.z, vec.w);
}

void _logMat2Impl(cstr name, Mat2 mat) {
    ssdkInitLog();
    saneLog->logFmt(SANE_LOG_INFO, "%s", name);
    u32 r = 0;
    SSDK_FORI(0, 4, 1) {
        printf("%0.1f ", mat.m[i]);
        if (r++ == 1) {
            printf("\n");
            r = 0;
        }
    }
}

void _logMat3Impl(cstr name, Mat3 mat) {
    ssdkInitLog();
    saneLog->logFmt(SANE_LOG_INFO, "%s", name);
    u32 r = 0;
    SSDK_FORI(0, 9, 1) {
        printf("%0.1f ", mat.m[i]);
        if (r++ == 2) {
            printf("\n");
            r = 0;
        }
    }
}

void _logMat4Impl(cstr name, Mat4 mat) {
    ssdkInitLog();
    saneLog->logFmt(SANE_LOG_INFO, "%s", name);
    u32 r = 0;
    SSDK_FORI(0, 16, 1) {
        printf("%0.1f ", mat.m[i]);
        if (r++ == 3) {
            printf("\n");
            r = 0;
        }
    }
}
/* ---------------- UTILITY ------------- */


/* ---------------- SCALAR -------------- */
f32 _toRadiansImpl(f32 deg) {
    return deg * SSDK_PI/180.0;
}
/* ---------------- SCALAR -------------- */


/* ---------------- VEC2 ---------------- */
// Creates a Vec2
Vec2 _newVec2Impl(f32 x, f32 y) {
    return (Vec2){x, y};
}

// Scales a Vec2 by a scalar value
Vec2 _scaleVec2Impl(Vec2 v, f32 scale) {
    return (Vec2){v.x * scale, v.y * scale};
}

// Adds two Vec2s
Vec2 _addVec2Impl(Vec2 v1, Vec2 v2) {
    return (Vec2){v1.x + v2.x, v1.y + v2.y};
}

// Subtracts one Vec2 from another
Vec2 _subVec2Impl(Vec2 v1, Vec2 v2) {
    return (Vec2){v1.x - v2.x, v1.y - v2.y};
}

// Dot product of two Vec2s
f32 _dotVec2Impl(Vec2 v1, Vec2 v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

// Return the magnitude of a Vec2
f32 _magVec2Impl(Vec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

// Normalize a Vec2
Vec2 _normVec2Impl(Vec2 v) {
    f32 length = saneMath->vector.mag2(v);
    return saneMath->vector.scale2(v, 1.0f / length);
}
/* ---------------- VEC2 ---------------- */


/* ---------------- VEC3 ---------------- */
// Creates a Vec3
Vec3 _newVec3Impl(f32 x, f32 y, f32 z) {
    return (Vec3){x, y, z};
}
// Scales a vec by a scalar value
Vec3 _scaleVec3Impl(Vec3 v, f32 scale) {
    Vec3 result;
    result.x = v.x * scale;
    result.y = v.y * scale;
    result.z = v.z * scale;
    return result;
}

// Adds two vectors
Vec3 _addVec3Impl(Vec3 v1, Vec3 v2) {
    Vec3 result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}

// Subtracts one vec from another
Vec3 _subVec3Impl(Vec3 v1, Vec3 v2) {
    Vec3 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

f32 _dotVec3Impl(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Return the magnitude of a Vec3
f32 _magVec3Impl(Vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Normalize a vec
Vec3 _normVec3Impl(Vec3 v) {
    f32 length = saneMath->vector.mag3(v);
    return saneMath->vector.scale3(v, 1.0f / length);
}

// Cross product of two vectors
Vec3 _crossVec3Impl(Vec3 v1, Vec3 v2) {
    Vec3 result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}
/* ---------------- VEC3 ---------------- */


/* ---------------- VEC4 ---------------- */
// Creates a Vec4
Vec4 _newVec4Impl(f32 x, f32 y, f32 z, f32 w) {
    return (Vec4){x, y, z, w};
}

// Scales a Vec4 by a scalar value
Vec4 _scaleVec4Impl(Vec4 v, f32 scale) {
    return (Vec4){v.x * scale, v.y * scale, v.z * scale, v.w * scale};
}

// Adds two Vec4s
Vec4 _addVec4Impl(Vec4 v1, Vec4 v2) {
    return (Vec4){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};
}

// Subtracts one Vec4 from another
Vec4 _subVec4Impl(Vec4 v1, Vec4 v2) {
    return (Vec4){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};
}

// Dot product of two Vec4s
f32 _dotVec4Impl(Vec4 v1, Vec4 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

// Return the magnitude of a Vec4
f32 _magVec4Impl(Vec4 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

// Normalize a Vec4
Vec4 _normVec4Impl(Vec4 v) {
    f32 length = saneMath->vector.mag4(v);
    return saneMath->vector.scale4(v, 1.0f / length);
}
/* ---------------- VEC4 ---------------- */


/* ---------------- MAT2 ---------------- */
Mat2 _identity2Impl(void) {
    return (Mat2){.m={
        1, 0,
        0, 1
    }};
}

Mat2 _transpose2Impl(Mat2 mat) {
    Mat2 result = mat;
    SSDK_SWAP(f32, result.m[1], result.m[2]);   // (0,1) <-> (1,0)
    return result;
}

Mat2 _mulMat2Impl(Mat2 a, Mat2 b) {
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
Mat3 _identity3Impl(void) {
    return (Mat3){.m={
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    }};
}

Mat3 _transpose3Impl(Mat3 mat) {
    Mat3 result = mat;
    SSDK_SWAP(f32, result.m[1], result.m[3]);   // (0,1) <-> (1,0)
    SSDK_SWAP(f32, result.m[2], result.m[6]);   // (0,2) <-> (2,0)
    SSDK_SWAP(f32, result.m[5], result.m[7]);   // (1,2) <-> (2,1)
    return result;
}

Mat3 _mulMat3Impl(Mat3 a, Mat3 b) {
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
Mat4 _identity4Impl(void) {
    return (Mat4){.m={
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    }};
}

Mat4 _transpose4Impl(Mat4 mat) {
    Mat4 result = mat;
    SSDK_SWAP(f32, result.m[1], result.m[4]);   // (0,1) <-> (1,0)
    SSDK_SWAP(f32, result.m[2], result.m[8]);   // (0,2) <-> (2,0)
    SSDK_SWAP(f32, result.m[3], result.m[12]);  // (0,3) <-> (3,0)
    SSDK_SWAP(f32, result.m[6], result.m[9]);   // (1,2) <-> (2,1)
    SSDK_SWAP(f32, result.m[7], result.m[13]);  // (1,3) <-> (3,1)
    SSDK_SWAP(f32, result.m[11],result.m[14]);  // (2,3) <-> (3,2)
    return result;
}

Vec3 _mult4v3Impl(Mat4 m, Vec3 v) {
    Vec3 result;
    result.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12];
    result.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13];
    result.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14];
    return result;
}

Mat4 _scale4Impl(f32 x, f32 y, f32 z) {
    Mat4 result = { .m={
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    }};
    return result;
}

Mat4 _trans4Impl(f32 x, f32 y, f32 z) {
    Mat4 result = { .m={
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    }};
    return result;
}

Mat4 _rotx4Impl(f32 angle) {
    f32 rad = saneMath->scalar.radians(angle);
    f32 cos_angle = cosf(rad);
    f32 sin_angle = sin(rad);
    Mat4 result = { .m={
        1, 0, 0, 0,
        0, cos_angle, sin_angle, 0,
        0,-sin_angle, cos_angle, 0,
        0, 0, 0, 1
    }};
    return result;
}

Mat4 _roty4Impl(f32 angle) {
    f32 rad = saneMath->scalar.radians(angle);
    f32 cos_angle = cosf(rad);
    f32 sin_angle = sin(rad);
    Mat4 result = { .m={
        cos_angle, 0,-sin_angle, 0,
        0, 1, 0, 0,
        sin_angle, 0, cos_angle, 0,
        0, 0, 0, 1
    }};
    return result;
}

Mat4 _rotz4Impl(f32 angle) {
    f32 rad = saneMath->scalar.radians(angle);
    f32 cos_angle = cosf(rad);
    f32 sin_angle = sin(rad);
    Mat4 result = { .m={
        cos_angle, sin_angle, 0, 0,
       -sin_angle, cos_angle, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    }};
    return result;
}

Mat4 _rot4Impl(Vec3 axis, f32 angle) {
    Mat4 result = saneMath->matrix.identity4();
    f32 rad = saneMath->scalar.radians(angle);
    f32 cos_theta = cosf(rad);
    f32 sin_theta = sinf(rad);

    f32 axis_len = saneMath->vector.mag3(axis);
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


Mat4 _mult4Impl(Mat4 a, Mat4 b) {
    Mat4 result = { .m={0}};
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            for (int i = 0; i < 4; ++i) {
                result.m[row * 4 + col] += a.m[row * 4 + i] * b.m[i * 4 + col];
            }
        }
    }
    return result;
}

Mat4 _lookatImpl(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 target = saneMath->vector.add3(eye, center);
    
    Vec3 f = saneMath->vector.norm3(saneMath->vector.sub3(target, eye)); 
    Vec3 s = saneMath->vector.norm3(saneMath->vector.cross3(f, up));
    Vec3 u = saneMath->vector.norm3(saneMath->vector.cross3(s, f));

    Mat4 result = { .m={
        s.x, u.x,-f.x, 0.0f,
        s.y, u.y,-f.y, 0.0f,
        s.z, u.z,-f.z, 0.0f,
       -saneMath->vector.dot3(s, eye), -saneMath->vector.dot3(u, eye), saneMath->vector.dot3(f, eye), 1.0f,
    }};
    
    return result;
}

Mat4 _orthoImpl(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
    f32 rml = right - left;
    f32 tmb = top - bottom;
    f32 fmn = far - near;
    
    f32 rpl = right + left;
    f32 tpb = top + bottom;
    f32 fpn = far + near;
    
    Mat4 result = { .m={
        2.0f / rml,  0.0f,       0.0f,       0.0f,
        0.0f,        2.0f / tmb, 0.0f,       0.0f,
        0.0f,        0,         -2.0f / fmn, 0.0f,
        -rpl / rml, -tpb / tmb, -fpn / fmn, 1.0f

    }};

    return result;
}

Mat4 _perspectiveImpl(f32 fovy, f32 aspect, f32 near, f32 far) {
    f32 tfov = tanf(saneMath->scalar.radians(fovy) / 2.0f);
    f32 fpn = far + near;
    f32 fmn = far - near;
    f32 fn = far * near;
    
    Mat4 result = { .m={
        1.0f / (aspect * tfov),   0,                0,                       0,
        0,                        1.0f / tfov,      0,                       0,
        0,                        0,               -fpn / fmn,              -1.0f,
        0,                        0,               -(2.0f * fn) / fmn,       1.0f
    }};

    return result;
}
/* ---------------- MAT4 ---------------- */


/* ---------------- API ---------------- */
byte ssdkInitMath(none) {
    if (saneMath != NULL) return SSDK_TRUE;

    saneMath = malloc(sizeof(SaneMath));
    if (!saneMath) return SSDK_FALSE;  // error: out of memory!

    saneMath->scalar.radians = _toRadiansImpl;

    saneMath->vector.log2 = _logVec2Impl;
    saneMath->vector.log3 = _logVec3Impl;
    saneMath->vector.log4 = _logVec4Impl;
    
    saneMath->vector.vec2 = _newVec2Impl;
    saneMath->vector.add2 = _addVec2Impl;
    saneMath->vector.sub2 = _subVec2Impl;
    saneMath->vector.dot2 = _dotVec2Impl;
    saneMath->vector.norm2 = _normVec2Impl;
    saneMath->vector.mag2 = _magVec2Impl;
    saneMath->vector.scale2 = _scaleVec2Impl;
    
    saneMath->vector.vec3 = _newVec3Impl;
    saneMath->vector.add3 = _addVec3Impl;
    saneMath->vector.sub3 = _subVec3Impl;
    saneMath->vector.dot3 = _dotVec3Impl;
    saneMath->vector.norm3 = _normVec3Impl;
    saneMath->vector.mag3 = _magVec3Impl;
    saneMath->vector.scale3 = _scaleVec3Impl;
    saneMath->vector.cross3 = _crossVec3Impl;
    
    saneMath->vector.vec4 = _newVec4Impl;
    saneMath->vector.add4 = _addVec4Impl;
    saneMath->vector.sub4 = _subVec4Impl;
    saneMath->vector.dot4 = _dotVec4Impl;
    saneMath->vector.norm4 = _normVec4Impl;
    saneMath->vector.mag4 = _magVec4Impl;
    saneMath->vector.scale4 = _scaleVec4Impl;
    
    saneMath->matrix.log2 = _logMat2Impl;
    saneMath->matrix.log3 = _logMat3Impl;
    saneMath->matrix.log4 = _logMat4Impl;

    // MATRIX 2
    saneMath->matrix.identity2 = _identity2Impl;
    saneMath->matrix.transpose2 = _transpose2Impl;
    
    saneMath->matrix.mult2 = _mulMat2Impl;
    
    // MATRIX 3
    saneMath->matrix.identity3 = _identity3Impl;
    saneMath->matrix.transpose3 = _transpose3Impl;
    
    saneMath->matrix.mult3 = _mulMat3Impl;

    // MATRIX 4
    saneMath->matrix.identity4 = _identity4Impl;
    saneMath->matrix.transpose4 = _transpose4Impl;
    
    saneMath->matrix.rot4 = _rot4Impl;
    saneMath->matrix.rotx4 = _rotx4Impl;
    saneMath->matrix.roty4 = _roty4Impl;
    saneMath->matrix.rotz4 = _rotz4Impl;
    saneMath->matrix.trans4 = _trans4Impl;
    saneMath->matrix.scale4 = _scale4Impl;
    
    saneMath->matrix.mult4 = _mult4Impl;
    saneMath->matrix.mult4v3 = _mult4v3Impl;
    
    saneMath->matrix.lookat = _lookatImpl;
    
    saneMath->matrix.ortho = _orthoImpl;
    saneMath->matrix.perspective = _perspectiveImpl;

    saneMath->module.mask = 0;
    saneMath->module.calls = 0;
    saneMath->module.name = "SaneMath";
    saneMath->module.size = sizeof(SaneMath);
    return SSDK_TRUE;
}

void ssdkExitMath(none) {
    if (saneMath == NULL) return;
    free(saneMath);
    saneMath = NULL;
}
/* ---------------- API ---------------- */
