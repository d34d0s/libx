#pragma once

#include <math.h>
#include "stdx_def.h"

/* ---------------- VECTOR ---------------- */
typedef enum Vector_Type {
    STDX_VEC2,
    STDX_VEC3,
    STDX_VEC4,
    STDX_VEC_TYPES,
} Vector_Type;

typedef struct Vector {
    Vector_Type type;
    union vec {
        f32 vec2[2];
        f32 vec3[3];
        f32 vec4[4];
    } vec;
} Vector;
/* ---------------- VECTOR ---------------- */

STDX_DEFINE_API(math,
    STDX_FNPTR(Vector, create_vector, Vector_Type type, f32* vector);
    STDX_FNPTR(Vector, scale_vector, f32 scalar, Vector* vector);
    STDX_FNPTR(void, print_vector, cstr name, Vector* vector);
    STDX_FNPTR(Vector, cross_vector, Vector* v1, Vector* v2);
    STDX_FNPTR(Vector, add_vector, Vector* v1, Vector* v2);
    STDX_FNPTR(Vector, sub_vector, Vector* v1, Vector* v2);
    STDX_FNPTR(f32, dot_vector, Vector* v1, Vector* v2);
    STDX_FNPTR(Vector, norm_vector, Vector* vector);
    STDX_FNPTR(f32, mag_vector, Vector* vector);
);
extern _stdx_math_api* math_api;

STDX_API u8 stdx_init_math(void);
STDX_API void stdx_cleanup_math(void);

