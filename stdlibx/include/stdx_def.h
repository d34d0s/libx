#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

typedef float			f32;
typedef double			f64;

typedef signed char		i8;
typedef signed short		i16;
typedef signed int 		i32;
typedef signed long long 	i64;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int 		u32;
typedef unsigned long long 	u64;

#ifdef DLL_EXPORT
    #ifdef _MSC_VER
        #define STDX_API __declspec(dllexport)
    #elif #defined (__GNUC__) || defined (__clang__)
        #define STDX_API __attribute__((visibility("default")))
    #else
        #define STDX_API
    #endif
#else
    #ifdef _MSC_VER
        #define STDX_API __declspec(dllimport)
    #elif defined(__GNUC__) || defined(__clang__)
        #define STDX_API __attribute__((visibility("default")))
    #else
        #define STDX_API
    #endif
#endif
