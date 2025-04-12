#ifndef __COREX_DEF_H__
#define __COREX_DEF_H__

#ifndef COREX_NO_STDLIB
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

#define COREX_TRUE   1
#define COREX_FALSE  0

#define COREX_SWAP(t, a, b) { t temp = a; (a) = (b); (b) = temp; }

#define COREX_HEAP_ARR(t, a) t* n
#define COREX_STACK_ARR(t, a, m) t* a[m]

#define COREX_IP2(x) ((x & (x - 1)) == 0)
#define COREX_ALIGN(p, a) (((p) + ((a) - 1)) & ~((a) - 1))

#define COREX_IGNORE(value)              ((void)v)
#define COREX_DEFAULT(type)              (type){0}
#define COREX_LITERAL(type, value)       (type){value}
#define COREX_DEREF(type, value)         *(type*)value
#define COREX_LITERAL_PTR(type, value)   &(type){value}

#define COREX_WRAPI(value, limit) ((value) % (limit))
#define COREX_WRAPF(value, limit) (fmod((f64)(value), (limit)))

#define COREX_MAX(a, b) ((a) > (b) ? a : b)
#define COREX_MIN(a, b) ((a) < (b) ? a : b)
#define COREX_CLAMP(value, min, max) ((value) > (max) ? (max) : (value) < (min) ? (min) : (value))

#define COREX_FORI(start, stop, step) for (u32 i = start; i < stop; i += step)
#define COREX_FORJ(start, stop, step) for (u32 j = start; j < stop; j += step)
#define COREX_FORK(start, stop, step) for (u32 k = start; k < stop; k += step)
#define COREX_FOR(type, iter, start, stop, step) for (type iter = start; iter < stop; iter += step)

#define COREX_KB  1024
#define COREX_MB  (COREX_KB * 1024)
#define COREX_GB  (COREX_MB * 1024)

#define COREX_FLIP_BIT(v, b)  ((v) ^ (1<<(b)))
#define COREX_SET_BITS(v, b)  ((v) | (1<<(b)))
#define COREX_GET_BITS(v, b)  ((v) & (1<<(b)))
#define COREX_REM_BITS(v, b)  ((v) & ~(1<<(b)))

#define COREX_FNPTR(t, n, ...) t (*n)(__VA_ARGS__)

#define COREX_IMPLEMENT(n, ...)          \
    void n##_impl(void) {               \
        __VA_ARGS__                     \
    }                                   \

#define COREX_DEFINE_ENUM(n, ...)        \
    typedef enum n {                    \
        __VA_ARGS__                     \
    } n                                 \

#define COREX_DEFINE_UNION(n, ...)       \
    typedef union n {                   \
        __VA_ARGS__                     \
    } n                                 \

#define COREX_DEFINE_STRUCT(n, ...)      \
    typedef struct n {                  \
        __VA_ARGS__                     \
    } n                                 \

#ifdef DLL_EXPORT
    #ifdef _MSC_VER
        #define COREX_EXPOSE __declspec(dllexport)
    #elif #defined (__GNUC__) || defined (__clang__)
        #define COREX_EXPOSE __attribute__((visibility("default")))
    #else
        #define COREX_EXPOSE
    #endif
#else
    #ifdef _MSC_VER
        #define COREX_EXPOSE __declspec(dllimport)
    #elif defined(__GNUC__) || defined(__clang__)
        #define COREX_EXPOSE __attribute__((visibility("default")))
    #else
        #define COREX_EXPOSE
    #endif
#endif

#endif  // COREX_DEF_H