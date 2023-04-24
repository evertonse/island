
#pragma once
#include <stdint.h>
#include <stdio.h>  
#include <assert.h>  

#include <string>  
#include <iostream>  
#include <sstream>  
#include <cmath>
#include <math.h>

// I saw this typing in rust and casey's stream, i really like it
typedef float	 f32;
typedef double   f64;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef uint8_t  byte;


typedef int64_t  i64;
typedef int32_t  i32;
typedef int16_t  i16;
typedef int8_t   i8;

# define M_PI   3.14159265358979323846 

#define HEADER 	"\033[95m"
#define OKBLUE 	"\033[94m"
#define OKCYAN 	"\033[96m"
#define OKGREEN	"\033[92m"
#define WARNING	"\033[93m"
#define FAIL 		"\033[91m"
#define ENDC 		"\033[0m"
#define PRINTBOLD 		"\033[1m"
#define UNDERLINE "\033[4m"

#define RED(msg) 	FAIL 	msg ENDC
#define GREEN(msg)	OKGREEN msg ENDC


#define RGBA_U32_RED(color)   (((color)&0x000000FF)>>(8*0))
#define RGBA_U32_GREEN(color) (((color)&0x0000FF00)>>(8*1))
#define RGBA_U32_BLUE(color)  (((color)&0x00FF0000)>>(8*2))
#define RGBA_U32_ALPHA(color) (((color)&0xFF000000)>>(8*3))
#define RGBA_U32_PACK(r, g, b, a) ((((r)&0xFF)<<(8*0)) | (((g)&0xFF)<<(8*1)) | (((b)&0xFF)<<(8*2)) | (((a)&0xFF)<<(8*3)))

// To be used when inside function and the data "persists" trough multiple calls (makes it ease to find later)
#define persistent_data static
#define filescope static
#define _notimplemented(msg) assert(0 && msg "Not Implemented.")  
#define _todo(msg) assert(0 && msg "To Do ! ")  

// Lower case macro are supposed to function like keyword, UPPERCASE macros are constans or simply macros taking function 

// Force inline and no-inline macros
//------------------------------------------------------------------------------------------------//

#if defined(__GNUC__) || defined(__GNUG__)
    #define force_inline inline __attribute__((always_inline))
    #define force_noinline  __attribute__((noinline))
#elif defined(_MSC_VER)
    #define force_inline  __forceinline
    #define force_noinline  __declspec(noinline)
#endif

#if defined(__GNUC__) || defined(__GNUG__)
#define force_restrict __restrict__
#elif defined(_MSC_VER)
#define force_restrict  __restrict
#endif

//------------------------------------------------------------------------------------------------//