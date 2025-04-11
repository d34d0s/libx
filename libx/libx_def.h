#ifndef __LIBX_DEF_H__
#define __LIBX_DEF_H__

#ifndef LIBX_NO_STDLIB
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#endif

typedef char*               str;
typedef const char*         cstr;

typedef float               f32;
typedef double              f64;

typedef signed char         i8;
typedef signed short        i16;
typedef signed int          i32;
typedef signed long long    i64;

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef intptr_t            iptr;
typedef uintptr_t           uptr;

#define LIBX_TRUE   1
#define LIBX_FALSE  0

#define LIBX_SWAP(t, a, b) { t temp = a; (a) = (b); (b) = temp; }

#define LIBX_HEAP_ARR(t, a) t* n
#define LIBX_STACK_ARR(t, a, m) t* a[m]

#define LIBX_IP2(x) ((x & (x - 1)) == 0)
#define LIBX_ALIGN(p, a) (((p) + ((a) - 1)) & ~((a) - 1))

#define LIBX_IGNORE(value)              ((void)v)
#define LIBX_DEFAULT(type)              (type){0}
#define LIBX_LITERAL(type, value)       (type){value}
#define LIBX_DEREF(type, value)         *(type*)value
#define LIBX_LITERAL_PTR(type, value)   &(type){value}

#define LIBX_WRAPI(value, limit) ((value) % (limit))
#define LIBX_WRAPF(value, limit) (fmod((f64)(value), (limit)))

#define LIBX_MAX(a, b) ((a) > (b) ? a : b)
#define LIBX_MIN(a, b) ((a) < (b) ? a : b)
#define LIBX_CLAMP(value, min, max) ((value) > (max) ? (max) : (value) < (min) ? (min) : (value))

#define LIBX_FORI(start, stop, step) for (u32 i = start; i < stop; i += step)
#define LIBX_FORJ(start, stop, step) for (u32 j = start; j < stop; j += step)
#define LIBX_FORK(start, stop, step) for (u32 k = start; k < stop; k += step)
#define LIBX_FOR(type, iter, start, stop, step) for (type iter = start; iter < stop; iter += step)

#define LIBX_KB  1024
#define LIBX_MB  (LIBX_KB * 1024)
#define LIBX_GB  (LIBX_MB * 1024)

#define LIBX_FLIP_BIT(v, b)  ((v) ^ (1<<(b)))
#define LIBX_SET_BITS(v, b)  ((v) | (1<<(b)))
#define LIBX_GET_BITS(v, b)  ((v) & (1<<(b)))
#define LIBX_REM_BITS(v, b)  ((v) & ~(1<<(b)))

#define LIBX_FNPTR(t, n, ...) t (*n)(__VA_ARGS__)

#define LIBX_IMPLEMENT(n, ...)          \
    void n##_impl(void) {               \
        __VA_ARGS__                     \
    }                                   \

#define LIBX_DEFINE_ENUM(n, ...)        \
    typedef enum n {                    \
        __VA_ARGS__                     \
    } n                                 \

#define LIBX_DEFINE_UNION(n, ...)       \
    typedef union n {                   \
        __VA_ARGS__                     \
    } n                                 \

#define LIBX_DEFINE_STRUCT(n, ...)      \
    typedef struct n {                  \
        __VA_ARGS__                     \
    } n                                 \

#ifdef DLL_EXPORT
    #ifdef _MSC_VER
        #define LIBX_EXPOSE __declspec(dllexport)
    #elif #defined (__GNUC__) || defined (__clang__)
        #define LIBX_EXPOSE __attribute__((visibility("default")))
    #else
        #define LIBX_EXPOSE
    #endif
#else
    #ifdef _MSC_VER
        #define LIBX_EXPOSE __declspec(dllimport)
    #elif defined(__GNUC__) || defined(__clang__)
        #define LIBX_EXPOSE __attribute__((visibility("default")))
    #else
        #define LIBX_EXPOSE
    #endif
#endif

#endif  // LIBX_DEF_H