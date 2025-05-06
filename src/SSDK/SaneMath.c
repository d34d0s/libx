#include <include/SSDK/SSDK.h>

SaneMath* saneMath = NULL;

/* ---------------- VECTOR ---------------- */
SaneVector _vector_impl(SaneVectorType t) {
    saneMath->module.calls++;
    switch(t) {
        case(SANE_VEC2i): return (SaneVector){ .type = t, .data.v2i = {0} };
        case(SANE_VEC3i): return (SaneVector){ .type = t, .data.v3i = {0} };
        case(SANE_VEC4i): return (SaneVector){ .type = t, .data.v4i = {0} };
        
        case(SANE_VEC2u): return (SaneVector){ .type = t, .data.v2u = {0} };
        case(SANE_VEC3u): return (SaneVector){ .type = t, .data.v3u = {0} };
        case(SANE_VEC4u): return (SaneVector){ .type = t, .data.v4u = {0} };
        
        case(SANE_VEC2f): return (SaneVector){ .type = t, .data.v2f = {0} };
        case(SANE_VEC3f): return (SaneVector){ .type = t, .data.v3f = {0} };
        case(SANE_VEC4f): return (SaneVector){ .type = t, .data.v4f = {0} };
        
        case SANE_VECTOR_TYPES: // fall through
        default: break;
    }
    return (SaneVector){ .type = SANE_VECTOR_TYPES, .data.v4i = {0} };
}
/* ---------------- VECTOR ---------------- */


/* ---------------- MATRIX ---------------- */
SaneMatrix _matrix_impl(SaneMatrixType t) {
    saneMath->module.calls++;
    switch(t) {
        case(SANE_MAT2i): return (SaneMatrix){ .type = t, .data.m2i = { {0} } };
        case(SANE_MAT3i): return (SaneMatrix){ .type = t, .data.m3i = { {0} } };
        case(SANE_MAT4i): return (SaneMatrix){ .type = t, .data.m4i = { {0} } };
        
        case(SANE_MAT2u): return (SaneMatrix){ .type = t, .data.m2u = { {0} } };
        case(SANE_MAT3u): return (SaneMatrix){ .type = t, .data.m3u = { {0} } };
        case(SANE_MAT4u): return (SaneMatrix){ .type = t, .data.m4u = { {0} } };
        
        case(SANE_MAT2f): return (SaneMatrix){ .type = t, .data.m2f = { {0} } };
        case(SANE_MAT3f): return (SaneMatrix){ .type = t, .data.m3f = { {0} } };
        case(SANE_MAT4f): return (SaneMatrix){ .type = t, .data.m4f = { {0} } };
        
        case SANE_VECTOR_TYPES: // fall through
        default: break;
    }
    return (SaneMatrix){ .type = SANE_VECTOR_TYPES, .data.m4i = { {0} } };
}
/* ---------------- MATRIX ---------------- */


/* ---------------- API ---------------- */
byte ssdkInitMath(none) {
    if (saneMath != NULL) return SSDK_TRUE;

    saneMath = malloc(sizeof(SaneMath));
    if (!saneMath) return SSDK_FALSE;  // error: out of memory!

    saneMath->vector = _vector_impl;
    saneMath->matrix = _matrix_impl;

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
