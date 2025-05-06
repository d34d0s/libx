#ifndef __SANEMATH_H__
#define __SANEMATH_H__

#include <include/SSDK/globals.h>

typedef enum SaneVectorType {
    SANE_VEC2i,
    SANE_VEC3i,
    SANE_VEC4i,
    
    SANE_VEC2u,
    SANE_VEC3u,
    SANE_VEC4u,
    
    SANE_VEC2f,
    SANE_VEC3f,
    SANE_VEC4f,

    SANE_VECTOR_TYPES
} SaneVectorType;
typedef struct SaneVector {
    u8 type;
    union {
        i32 v2i[2];
        i32 v3i[3];
        i32 v4i[4];
        
        u32 v2u[2];
        u32 v3u[3];
        u32 v4u[4];
        
        f32 v2f[2];
        f32 v3f[3];
        f32 v4f[4];
    } data;
} SaneVector;

typedef enum SaneMatrixType {
    SANE_MAT2i,
    SANE_MAT3i,
    SANE_MAT4i,
    
    SANE_MAT2u,
    SANE_MAT3u,
    SANE_MAT4u,
    
    SANE_MAT2f,
    SANE_MAT3f,
    SANE_MAT4f,
    
    SANE_MATRIX_TYPES
} SaneMatrixType;
typedef struct SaneMatrix {
    u8 type;
    union {
        i32 m2i[2][2];
        i32 m3i[3][3];
        i32 m4i[4][4];

        u32 m2u[2][2];
        u32 m3u[3][3];
        u32 m4u[4][4];

        f32 m2f[2][2];
        f32 m3f[3][3];
        f32 m4f[4][4];
    } data;
} SaneMatrix;

typedef struct SaneMath {
    SaneModule module;
    
    SSDK_FNPTR(SaneVector, vector, SaneVectorType t);
    SSDK_FNPTR(SaneMatrix, matrix, SaneMatrixType t);
} SaneMath;
extern SaneMath* saneMath;

SSDK_FUNC byte ssdkInitMath(none);
SSDK_FUNC void ssdkExitMath(none);

#endif  // __SANEMATH_H__