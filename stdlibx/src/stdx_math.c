#include "../include/stdx_math.h"
#include "../include/stdx_memory.h"

_stdx_math_api* math_api = NULL;

/* ---------------- VECTOR ---------------- */
void _print_vector_impl(cstr name, Vector* vector) {
    switch (vector->type) {
    case STDX_VEC2:
        printf("%s: <%0.1f, %0.1f>\n", name,
            vector->vec.vec2[0], vector->vec.vec2[1]);
        break;
    case STDX_VEC3:
        printf("%s: <%0.1f, %0.1f, %0.1f>\n", name,
            vector->vec.vec3[0], vector->vec.vec3[1], vector->vec.vec3[2]);
        break;
    case STDX_VEC4:
        printf("%s: <%0.1f, %0.1f, %0.1f, %0.1f>\n", name,
            vector->vec.vec4[0], vector->vec.vec4[1], vector->vec.vec4[2], vector->vec.vec4[3]);
        break;
    default:
        break;
    }
}

Vector _create_vector_impl(Vector_Type type, f32* vector) {
    switch (type) {
    case STDX_VEC2:
        return (Vector){ .type = type, .vec.vec2={vector[0], vector[1]} };
        break;
    case STDX_VEC3:
        return (Vector){ .type = type, .vec.vec3={vector[0], vector[1], vector[2]} };
        break;
    case STDX_VEC4:
        return (Vector){ .type = type, .vec.vec4={vector[0], vector[1], vector[2], vector[3]} };
        break;
    default:
        break;
    }
}

Vector _add_vector_impl(Vector* v1, Vector* v2) {
    switch (v1->type) {
    case STDX_VEC2:
        return (Vector){ .type = v1->type, .vec.vec2={
            v1->vec.vec2[0] + v2->vec.vec2[0],
            v1->vec.vec2[1] + v2->vec.vec2[1]
        } };
        break;
    case STDX_VEC3:
        return (Vector){ .type = v1->type, .vec.vec3={
            v1->vec.vec3[0] + v2->vec.vec3[0],
            v1->vec.vec3[1] + v2->vec.vec3[1],
            v1->vec.vec3[2] + v2->vec.vec3[2]
        } };
        break;
    case STDX_VEC4:
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
    case STDX_VEC2:
        return (Vector){ .type = v1->type, .vec.vec2={
            v1->vec.vec2[0] - v2->vec.vec2[0],
            v1->vec.vec2[1] - v2->vec.vec2[1]
        } };
        break;
    case STDX_VEC3:
        return (Vector){ .type = v1->type, .vec.vec3={
            v1->vec.vec3[0] - v2->vec.vec3[0],
            v1->vec.vec3[1] - v2->vec.vec3[1],
            v1->vec.vec3[2] - v2->vec.vec3[2]
        } };
        break;
    case STDX_VEC4:
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
    case STDX_VEC2:
        return (Vector){ .type = vector->type, .vec.vec2={
            vector->vec.vec2[0] * scalar, vector->vec.vec2[1] * scalar} };
        break;
    case STDX_VEC3:
        return (Vector){ .type = vector->type, .vec.vec3={
            vector->vec.vec3[0] * scalar, vector->vec.vec3[1] * scalar,
            vector->vec.vec3[2] * scalar} };
        break;
    case STDX_VEC4:
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
    case STDX_VEC3:
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
    case STDX_VEC2:
        return (v1->vec.vec2[0] * v2->vec.vec2[0] + v1->vec.vec2[1] * v2->vec.vec2[1]);
        break;
    case STDX_VEC3:
        return (v1->vec.vec3[0] * v2->vec.vec3[0] + v1->vec.vec3[1] * v2->vec.vec3[1] + 
                v1->vec.vec3[2] * v2->vec.vec3[2]);
        break;
        case STDX_VEC4:
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
    case STDX_VEC2:
        return (Vector){ .type = vector->type, .vec.vec2={
            vector->vec.vec2[0] / magnitude, vector->vec.vec2[1] / magnitude} };
        break;
    case STDX_VEC3:
        return (Vector){ .type = vector->type, .vec.vec3={
            vector->vec.vec3[0] / magnitude, vector->vec.vec3[1] / magnitude,
            vector->vec.vec3[2] / magnitude} };
        break;
    case STDX_VEC4:
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
    case STDX_VEC2:
        return sqrtf(vector->vec.vec2[0] * vector->vec.vec2[0] + vector->vec.vec2[1] * vector->vec.vec2[1]);
        break;
    case STDX_VEC3:
        return sqrtf(vector->vec.vec3[0] * vector->vec.vec3[0] + vector->vec.vec3[1] * vector->vec.vec3[1] + 
                vector->vec.vec3[2] * vector->vec.vec3[2]);
        break;
        case STDX_VEC4:
        return sqrtf(vector->vec.vec4[0] * vector->vec.vec4[0] + vector->vec.vec4[1] * vector->vec.vec4[1] +
                vector->vec.vec4[2] * vector->vec.vec4[2] + vector->vec.vec4[3] * vector->vec.vec4[3]);
        break;
    default:
        break;
    }
}
/* ---------------- VECTOR ---------------- */

u8 stdx_init_math(void) {
    math_api = memory_api->alloc(sizeof(_stdx_math_api), 16);
    if (!math_api) return STDX_FALSE;   // error: out of memory!

    math_api->create_vector = _create_vector_impl;
    math_api->scale_vector = _scale_vector_impl;
    math_api->print_vector = _print_vector_impl;
    math_api->cross_vector = _cross_vector_impl;
    math_api->add_vector = _add_vector_impl;
    math_api->sub_vector = _sub_vector_impl;
    math_api->dot_vector = _dot_vector_impl;
    math_api->norm_vector = _norm_vector_impl;
    math_api->mag_vector = _mag_vector_impl;

    return STDX_TRUE;
}

void stdx_cleanup_math(void) {
    memory_api->dealloc(math_api);
}

