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

typedef struct _stdx_math_api {
    Vector (*create_vector)(Vector_Type type, f32* vector);
    Vector (*scale_vector)(f32 scalar, Vector* vector);
    void (*print_vector)(cstr name, Vector* vector);
    Vector (*cross_vector)(Vector* v1, Vector* v2);
    Vector (*add_vector)(Vector* v1, Vector* v2);
    Vector (*sub_vector)(Vector* v1, Vector* v2);
    f32 (*dot_vector)(Vector* v1, Vector* v2);
    Vector (*norm_vector)(Vector* vector);
    f32 (*mag_vector)(Vector* vector);
} _stdx_math_api;
extern _stdx_math_api* math_api;

STDX_API u8 stdx_init_math(void);
STDX_API void stdx_cleanup_math(void);
