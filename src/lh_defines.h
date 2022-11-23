#include "stdint.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;
typedef int32_t b32;
typedef int8_t b8;

#define global_variable static
#define local_persist static
#define internal static

#define TRUE 1
#define FALSE 0

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof((array)[0]))
#define ASSERT(condition) if(!(condition)) {*(i32 *)0 = 0;}

#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)

#define FPS 30.0f
#define TARGET_SECONDS_PER_FRAME (1.0f / FPS)

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

//#define WINDOW_WIDTH 1920
//#define WINDOW_HEIGHT 1080

// SSE2
#define M(a, i) ((f32 *)&(a))[i]
#define Mi(a, i) ((i32 *)&(a))[i]
#define Mu(a, i) ((u32 *)&(a))[i]
