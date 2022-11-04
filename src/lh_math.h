#ifndef _LH_MATH_H_
#define _LH_MATH_H_

#include "lh_defines.h"
#include <xmmintrin.h>
#include <math.h>
#include <float.h>

#define EPSILON 0.000001f
#define PI 3.14159265359f
#define RAD(value) (value*(PI/180.0f))
#define DEG(value) (value*(180.0f/PI))
#define CMP(x, y) (fabsf((x) - (y)) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

struct vec2 {
    union {
        struct {
            f32 x;
            f32 y;
        };
        f32 v[2];
    }; 
};

struct vec3 {
    union {
        struct {
            f32 x;
            f32 y;
            f32 z;
        };
        f32 v[3];
    }; 
};

typedef vec3 Point;

struct vec4 {
    union {
        struct {
            f32 x;
            f32 y;
            f32 z;
            f32 w;
        };
        f32 v[4];
        __m128 wide;
    }; 
};

typedef vec4 quat;

struct mat2 {
    union {
        struct {
            f32 m00; f32 m01;
            f32 m10; f32 m11;
        };
        f32 m[4];
    };
};

struct mat3 {
    union {
        struct {
            f32 m00; f32 m01; f32 m02;
            f32 m10; f32 m11; f32 m12;
            f32 m20; f32 m21; f32 m22;
        };
        f32 m[9];
    };
};

struct mat4 {
    union {
        struct {
            f32 m00; f32 m01; f32 m02; f32 m03;
            f32 m10; f32 m11; f32 m12; f32 m13;
            f32 m20; f32 m21; f32 m22; f32 m23;
            f32 m30; f32 m31; f32 m32; f32 m33;
        };
        f32 m[16];
        __m128 row[4];

    };
};

struct rectangle2i {
    i32 minX, minY;
    i32 maxX, maxY;
};

struct Transform {
    vec3 position;
    vec3 rotation;
    vec3 scale; 
};

rectangle2i RectangleInvertedInfinity();
rectangle2i RectangleIntersect(rectangle2i a, rectangle2i b);
rectangle2i RectangleUnion(rectangle2i a, rectangle2i b);
i32 RectangleGetClampArea(rectangle2i a);
bool RectangleHasArea(rectangle2i a);

mat4 TransformToMat4(vec3 position, vec3 rotation, vec3 scale);

f32 lerp(f32 a, f32 b, f32 t);

vec2 operator+(vec2 l, vec2 r);
vec2 operator-(vec2 l, vec2 r);
vec2 operator*(vec2 l, vec2 r);
vec2 operator*(vec2 v, f32 f);
vec2 operator/(vec2 l, vec2 r);
f32 dot(vec2 l, vec2 r);
f32 lenSq(vec2 v);
f32 len(vec2 v);
void normalize(vec2 *v);
vec2 normalized(vec2 v);
f32 angle(vec2 l, vec2 r);
vec2 project(vec2 a, vec2 b);
vec2 reject(vec2 a, vec2 b);
vec2 reflect(vec2 a, vec2 b);
vec2 lerp(vec2 a, vec2 b, f32 t);
vec2 nlerp(vec2 a, vec2 b, f32 t);
vec2 slerp(vec2 s, vec2 e, f32 t);

vec3 operator+(vec3 l, vec3 r);
vec3 operator-(vec3 l, vec3 r);
vec3 operator*(vec3 l, vec3 r);
vec3 operator*(vec3 v, f32 f);
vec3 operator/(vec3 l, vec3 r);
f32 dot(vec3 l, vec3 r);
f32 lenSq(vec3 v);
f32 len(vec3 v);
void normalize(vec3 *v);
vec3 normalized(vec3 v);
f32 angle(vec3 l, vec3 r);
vec3 project(vec3 a, vec3 b);
vec3 reject(vec3 a, vec3 b);
vec3 reflect(vec3 a, vec3 b);
vec3 cross(vec3 l, vec3 r);
vec3 lerp(vec3 a, vec3 b, f32 t);
vec3 nlerp(vec3 a, vec3 b, f32 t);
vec3 slerp(vec3 s, vec3 e, f32 t);

mat2 Mat2Identity();
void Mat2Print(mat2 m);
mat2 Mat2Rotate(f32 angle);
mat2 operator+(mat2 a, mat2 b);
mat2 operator*(mat2 m, f32 f);
vec2 operator*(mat2 m, vec2 v);
mat2 operator*(mat2 a, mat2 b);

mat3 Mat3Identity();
void Mat3Print(mat3 m);
mat3 Mat3Scale(f32 x, f32 y, f32 z);
mat3 Mat3RotateX(f32 angle);
mat3 Mat3RotateY(f32 angle);
mat3 Mat3RotateZ(f32 angle);
mat3 operator+(mat3 a, mat3 b);
mat3 operator*(mat3 m, f32 f);
vec3 operator*(mat3 m, vec3 v);
mat3 operator*(mat3 a, mat3 b);

mat4 Mat4Identity();
void Mat4Print(mat4 m);
mat4 Mat4Translate(f32 x, f32 y, f32 z);
mat4 Mat4Scale(f32 x, f32 y, f32 z);
mat4 Mat4RotateX(f32 angle);
mat4 Mat4RotateY(f32 angle);
mat4 Mat4RotateZ(f32 angle);
mat4 Mat4Frustum(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f);
mat4 Mat4Perspective(f32 fov, f32 aspect, f32 znear, f32 zfar);
mat4 Mat4Ortho(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f);
mat4 Mat4LookAt(vec3 position, vec3 target, vec3 up);
mat4 operator+(mat4 a, mat4 b);
mat4 operator*(mat4 m, f32 f);
vec4 operator*(mat4 m, vec4 v);
mat4 operator*(mat4 a, mat4 b);
mat4 Mat4MulSSE(mat4 a, mat4 b);

vec3 Vec4ToVec3(vec4 v);
vec4 Vec3ToVec4(vec3 v, f32 w);

#endif
