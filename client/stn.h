/*
   stn.h - v1.1.2

   ----------------------------------------------------------------------------

   MIT License

   Copyright (c) 2021 Nullsson

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#ifndef STN_H
#define STN_H

#if defined(__clang__)
    #define STN_COMPILER_CLANG
    //#include <x86intrin.h> // TODO(Oskar): Add emscripten flag here to check if compiling for emscripten.
#elif defined(__GNUC__) || defined(__GNUG__)
    #define STN_COMPILER_GCC
    #include <x86intrin.h>
#elif defined(_WIN32) && !defined(__MINGW32__)
    #ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #endif

    #ifndef _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_NONSTDC_NO_DEPRECATE
    #endif

    #ifndef _CRT_NON_CONFORMING_SWPRINTFS
    #define _CRT_NON_CONFORMING_SWPRINTFS
    #endif

    #if defined(_MSC_VER)
        #define STN_COMPILER_MSVC // TODO(Oskar): Check for and support more compilers for intrin.
        #include <intrin.h>
    #endif
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <time.h>

#ifndef STN_NO_SSE
    #ifdef _MSC_VER
        // NOTE(Oskar): MSVC supports SSE in amd64 mode or _M_IX86_FP >= 1 (2 means SSE2)
        #if defined(_M_AMD64) || ( defined(_M_IX86_FP) && _M_IX86_FP >= 1)
            #define STN_USE_SSE 1
        #endif
        #else // NOTE(Oskar): Not MSVC but instead GCC / Clang or something.
        #ifdef __SSE__
            #define STN_USE_SSE 1
        #endif
    #endif
#endif

#if STN_USE_SSE
    #include <xmmintrin.h>
#endif

#define STN_GLOBAL          static
#define STN_INTERNAL        static
#define STN_LOCAL_PERSIST   static

#define MemoryCopy memcpy
#define MemoryMove memmove
#define MemorySet  memset
#define CalculateCStringLength (u32)strlen
#define FMod fmodf
#define PowF powf

// NOTE(Oskar): Had to add this define for legacy projects of mine that already
//              have all of theese typedefs.
#ifndef STN_NO_TYPES
   typedef int8_t   i8;
   typedef int16_t  i16;
   typedef int32_t  i32;
   typedef int64_t  i64;
   typedef uint8_t  u8;
   typedef uint16_t u16;
   typedef uint32_t u32;
   typedef uint64_t u64;
   typedef i8       s8;
   typedef i16      s16;
   typedef i32      s32;
   typedef i64      s64;
   typedef i8       b8;
   typedef i16      b16;
   typedef i32      b32;
   typedef i64      b64;
   typedef float    f32;
   typedef double   f64;

   typedef uintptr_t       umm;
   typedef intptr_t        smm;
#endif

#define STN_U16MAX 65535
#define STN_I32MIN ((i32)0x80000000)
#define STN_I32MAX ((i32)0x7fffffff)
#define STN_U32MIN 0
#define STN_U32MAX ((u32)-1)
#define STN_U64MAX ((u64)-1)
#define STN_F32MAX FLT_MAX
#define STN_F32MIN -FLT_MAX

#define STN_Bytes(n)      (n)
#define STN_Kilobytes(n)  (STN_Bytes(n)*1024)
#define STN_Megabytes(n)  (STN_Kilobytes(n)*1024)
#define STN_Gigabytes(n)  (STN_Megabytes(n)*1024)

#define STN_Align4(Value)   ((Value + 3) & ~3)
#define STN_Align8(Value)   ((Value + 7) & ~7)
#define STN_Align16(Value)  ((Value + 15) & ~15)

#define STN_Min(a, b) ((a) < (b) ? (a) : (b))
#define STN_Max(a, b) ((a) > (b) ? (a) : (b))

#define STN_ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

#ifdef STN_USE_ALL
   #include "stn_intrin.h"
   #include "stn_math.h"
   #include "stn_string.h"
   #include "stn_memory.h"
   #include "stn_random.h"
#else
   #ifdef STN_USE_INTRIN
   #include "stn_intrin.h"
   #endif

   #ifdef STN_USE_MATH
   #include "stn_math.h"
   #endif

   #ifdef STN_USE_STRING
   #include "stn_string.h"
   #endif

   #ifdef STN_USE_MEMORY
   #include "stn_memory.h"
   #endif

   #ifdef STN_USE_RANDOM
   #ifndef STN_USE_MATH
   #include "stn_math.h"
   #endif
   #include "stn_random.h"
   #endif
#endif

#endif // STN_H
