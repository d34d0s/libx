#include "../include/libx_math.h"
#include "../include/libx_memory.h"

_libx_math_api* math_api = NULL;

/* ---------------- VECTOR ---------------- */
void _print_vector_impl(cstr name, Vector* vector) {
    switch (vector->type) {
    case LIBX_VEC2:
        printf("%s: <%0.1f, %0.1f>\n", name,
            vector->vec.vec2[0], vector->vec.vec2[1]);
        break;
    case LIBX_VEC3:
        printf("%s: <%0.1f, %0.1f, %0.1f>\n", name,
            vector->vec.vec3[0], vector->vec.vec3[1], vector->vec.vec3[2]);
        break;
    case LIBX_VEC4:
        printf("%s: <%0.1f, %0.1f, %0.1f, %0.1f>\n", name,
            vector->vec.vec4[0], vector->vec.vec4[1], vector->vec.vec4[2], vector->vec.vec4[3]);
        break;
    default:
        break;
    }
}

Vector _create_vector_impl(Vector_Type type, f32* vector) {
    switch (type) {
    case LIBX_VEC2:
        if (!vector) return (Vector){ .type = type, .vec.vec2={0} };
        return (Vector){ .type = type, .vec.vec2={vector[0], vector[1]} };
        break;
    case LIBX_VEC3:
        if (!vector) return (Vector){ .type = type, .vec.vec3={0} };
        return (Vector){ .type = type, .vec.vec3={vector[0], vector[1], vector[2]} };
        break;
    case LIBX_VEC4:
        if (!vector) return (Vector){ .type = type, .vec.vec4={0} };
        return (Vector){ .type = type, .vec.vec4={vector[0], vector[1], vector[2], vector[3]} };
        break;
    default:
        break;
    }
}

Vector _add_vector_impl(Vector* v1, Vector* v2) {
    switch (v1->type) {
    case LIBX_VEC2:
        return (Vector){ .type = v1->type, .vec.vec2={
            v1->vec.vec2[0] + v2->vec.vec2[0],
            v1->vec.vec2[1] + v2->vec.vec2[1]
        } };
        break;
    case LIBX_VEC3:
        return (Vector){ .type = v1->type, .vec.vec3={
            v1->vec.vec3[0] + v2->vec.vec3[0],
            v1->vec.vec3[1] + v2->vec.vec3[1],
            v1->vec.vec3[2] + v2->vec.vec3[2]
        } };
        break;
    case LIBX_VEC4:
        return (Vector){ .type = v1->type, .vec.vec4={
            v1->vec.vec4[0] + v2->vec.vec4[0],
            v1->vec.vec4[1] + v2->vec.vec4[1],
            v1->vec.vec4[2] + v2->vec.vec4[2],
            v1->vec.vec4[3] + v2->vec.vec4[3]
        } };
        break;
    default:
        break;
    }
}

Vector _sub_vector_impl(Vector* v1, Vector* v2) {
    switch (v1->type) {
    case LIBX_VEC2:
        return (Vector){ .type = v1->type, .vec.vec2={
            v1->vec.vec2[0] - v2->vec.vec2[0],
            v1->vec.vec2[1] - v2->vec.vec2[1]
        } };
        break;
    case LIBX_VEC3:
        return (Vector){ .type = v1->type, .vec.vec3={
            v1->vec.vec3[0] - v2->vec.vec3[0],
            v1->vec.vec3[1] - v2->vec.vec3[1],
            v1->vec.vec3[2] - v2->vec.vec3[2]
        } };
        break;
    case LIBX_VEC4:
        return (Vector){ .type = v1->type, .vec.vec4={
            v1->vec.vec4[0] - v2->vec.vec4[0],
            v1->vec.vec4[1] - v2->vec.vec4[1],
            v1->vec.vec4[2] - v2->vec.vec4[2],
            v1->vec.vec4[3] - v2->vec.vec4[3]
        } };
        break;
    default:
        break;
    }
}

Vector _scale_vector_impl(f32 scalar, Vector* vector) {
    switch (vector->type) {
    case LIBX_VEC2:
        return (Vector){ .type = vector->type, .vec.vec2={
            vector->vec.vec2[0] * scalar, vector->vec.vec2[1] * scalar} };
        break;
    case LIBX_VEC3:
        return (Vector){ .type = vector->type, .vec.vec3={
            vector->vec.vec3[0] * scalar, vector->vec.vec3[1] * scalar,
            vector->vec.vec3[2] * scalar} };
        break;
    case LIBX_VEC4:
        return (Vector){ .type = vector->type, .vec.vec4={
            vector->vec.vec4[0] * scalar, vector->vec.vec4[1] * scalar,
            vector->vec.vec4[2] * scalar, vector->vec.vec4[3] * scalar} };
        break;
    default:
        break;
    }
}

Vector _cross_vector_impl(Vector* v1, Vector* v2) {
    switch (v1->type) {
    case LIBX_VEC3:
        return (Vector){ .type = v1->type, .vec.vec3={
            v1->vec.vec3[2] * v2->vec.vec3[3] - v1->vec.vec3[3] * v2->vec.vec3[2],
            v1->vec.vec3[3] * v2->vec.vec3[1] - v1->vec.vec3[1] * v2->vec.vec3[3],
            v1->vec.vec3[1] * v2->vec.vec3[2] - v1->vec.vec3[2] * v2->vec.vec3[1]} };
        break;
    default:
        break;
    }
}

f32 _dot_vector_impl(Vector* v1, Vector* v2) {
    switch (v1->type) {
    case LIBX_VEC2:
        return (v1->vec.vec2[0] * v2->vec.vec2[0] + v1->vec.vec2[1] * v2->vec.vec2[1]);
        break;
    case LIBX_VEC3:
        return (v1->vec.vec3[0] * v2->vec.vec3[0] + v1->vec.vec3[1] * v2->vec.vec3[1] + 
                v1->vec.vec3[2] * v2->vec.vec3[2]);
        break;
        case LIBX_VEC4:
        return (v1->vec.vec4[0] * v2->vec.vec4[0] + v1->vec.vec4[1] * v2->vec.vec4[1] +
                v1->vec.vec4[2] * v2->vec.vec4[2] + v1->vec.vec4[3] * v2->vec.vec4[3]);
        break;
    default:
        break;
    }
}

Vector _norm_vector_impl(Vector* vector) {
    f32 magnitude = math_api->mag_vector(vector);
    switch (vector->type) {
    case LIBX_VEC2:
        return (Vector){ .type = vector->type, .vec.vec2={
            vector->vec.vec2[0] / magnitude, vector->vec.vec2[1] / magnitude} };
        break;
    case LIBX_VEC3:
        return (Vector){ .type = vector->type, .vec.vec3={
            vector->vec.vec3[0] / magnitude, vector->vec.vec3[1] / magnitude,
            vector->vec.vec3[2] / magnitude} };
        break;
    case LIBX_VEC4:
        return (Vector){ .type = vector->type, .vec.vec4={
            vector->vec.vec4[0] / magnitude, vector->vec.vec4[1] / magnitude,
            vector->vec.vec4[2] / magnitude, vector->vec.vec4[3] / magnitude} };
        break;
    default:
        break;
    }
}

f32 _mag_vector_impl(Vector* vector) {
    switch (vector->type) {
    case LIBX_VEC2:
        return sqrtf(vector->vec.vec2[0] * vector->vec.vec2[0] + vector->vec.vec2[1] * vector->vec.vec2[1]);
        break;
    case LIBX_VEC3:
        return sqrtf(vector->vec.vec3[0] * vector->vec.vec3[0] + vector->vec.vec3[1] * vector->vec.vec3[1] + 
                vector->vec.vec3[2] * vector->vec.vec3[2]);
        break;
        case LIBX_VEC4:
        return sqrtf(vector->vec.vec4[0] * vector->vec.vec4[0] + vector->vec.vec4[1] * vector->vec.vec4[1] +
                vector->vec.vec4[2] * vector->vec.vec4[2] + vector->vec.vec4[3] * vector->vec.vec4[3]);
        break;
    default:
        break;
    }
}
/* ---------------- VECTOR ---------------- */

/* ---------------- MATRIX 4 ---------------- */
void _pirnt_matrix4_impl(cstr name, Matrix4* matrix4) {
    printf("%s\n", name);
    u32 r = 0;
    LIBX_FORI(0, 16, 1) {
        printf("%0.3f ", matrix4->mat[i]);
        if (r++ == 3) {
            printf("\n");
            r = 0;
        }
    }
}

Matrix4 _identity_matrix4_impl(void) {
    return (Matrix4){ .mat = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    }};
}

Matrix4 _create_matrix4_impl(f32* matrix4) {
    if (!matrix4) return math_api->identity_matrix4();
    Matrix4 matrix;
    LIBX_FORI(0, 16, 1) matrix.mat[i] = matrix4[i];
    return matrix;
}

Matrix4 _rot_matrix4_impl(f32 x, f32 y, f32 z) {}

Matrix4 _trans_matrix4_impl(f32 x, f32 y, f32 z) {}

Matrix4 _scale_matrix4_impl(f32 x, f32 y, f32 z) {}

Matrix4 _mult_matrix4_impl(Matrix4 m1, Matrix4 m2) {
    Matrix4 result = {0};

    LIBX_FORI(0, 4, 1) {
        LIBX_FORJ(0, 4, 1) {
            LIBX_FORK(0, 4, 1) {
                result.mat[i * 4 + j] += m1.mat[i * 4 + k] * m2.mat[k * 4 + j];
            }
        }
    }
    return result;
}

Vector _mult_matrix4_vector3_impl(Matrix4 matrix4, Vector vector) {
    if (vector.type != LIBX_VEC3) return (Vector){ .type = LIBX_VEC_TYPES };

    return (Vector){ .type = LIBX_VEC3, .vec.vec3 = {
        matrix4.mat[0] * vector.vec.vec3[0] + matrix4.mat[4] * vector.vec.vec3[1] + matrix4.mat[8] * vector.vec.vec3[2] + matrix4.mat[12],
        matrix4.mat[1] * vector.vec.vec3[0] + matrix4.mat[5] * vector.vec.vec3[1] + matrix4.mat[9] * vector.vec.vec3[2] + matrix4.mat[13],
        matrix4.mat[2] * vector.vec.vec3[0] + matrix4.mat[6] * vector.vec.vec3[1] + matrix4.mat[10] * vector.vec.vec3[2] + matrix4.mat[14]
    }};
}

Matrix4 _look_at_impl(Vector eye, Vector center, Vector up) {}

Matrix4 _perspective_impl(f32 fov, f32 aspect, f32 near, f32 far) {}

Matrix4 _ortho_impl(f32 left, f32 top, f32 right, f32 bottom, f32 near, f32 far) {}

/* ---------------- MATRIX 4 ---------------- */

u8 libx_init_math(void) {
    math_api = memory_api->alloc(sizeof(_libx_math_api), 16);
    if (!math_api) return LIBX_FALSE;   // error: out of memory!

    math_api->create_vector = _create_vector_impl;
    math_api->scale_vector = _scale_vector_impl;
    math_api->print_vector = _print_vector_impl;
    math_api->cross_vector = _cross_vector_impl;
    math_api->add_vector = _add_vector_impl;
    math_api->sub_vector = _sub_vector_impl;
    math_api->dot_vector = _dot_vector_impl;
    math_api->norm_vector = _norm_vector_impl;
    math_api->mag_vector = _mag_vector_impl;
    
    math_api->print_matrix4 = _pirnt_matrix4_impl;
    math_api->identity_matrix4 = _identity_matrix4_impl;
    math_api->create_matrix4 = _create_matrix4_impl;
    math_api->rot_matrix4 = _rot_matrix4_impl;
    math_api->trans_matrix4 = _trans_matrix4_impl;
    math_api->scale_matrix4 = _scale_matrix4_impl;
    math_api->mult_matrix4 = _mult_matrix4_impl;
    math_api->mult_matrix4_vector3 = _mult_matrix4_vector3_impl;
    math_api->look_at = _look_at_impl;
    math_api->perspective = _perspective_impl;
    math_api->ortho = _ortho_impl;

    return LIBX_TRUE;
}

void libx_cleanup_math(void) {
    memory_api->dealloc(math_api);
}

