#include "lh_math.h"

#include <math.h>
#include <windows.h>
#include <stdio.h>
#include <limits.h>

// F32 FUNCTIONS

f32 lerp(f32 a, f32 b, f32 t) {
    f32 result = a + (b - a) * t;
    return result;
}


// VEC2 FUNCTIONS
vec2 operator+(vec2 l, vec2 r) {
    vec2 result = { l.x + r.x, l.y + r.y };
    return result;
}

vec2 operator-(vec2 l, vec2 r) {
    vec2 result = { l.x - r.x, l.y - r.y };
    return result;
}

vec2 operator*(vec2 l, vec2 r) {
    vec2 result = { l.x * r.x, l.y * r.y };
    return result;
}

vec2 operator*(vec2 v, f32 f) {
    vec2 result = { v.x * f, v.y * f };
    return result;
}

vec2 operator/(vec2 l, vec2 r) {
    vec2 result = { l.x / r.x, l.y / r.y };
    return result;
}

f32 dot(vec2 l, vec2 r) {
    return l.x * r.x + l.y * r.y;
}

f32 lenSq(vec2 v) {
    return v.x * v.x + v.y * v.y;
}

f32 len(vec2 v) {
    f32 lenSq = v.x * v.x + v.y * v.y;
    if(lenSq < EPSILON) {
        return 0.0f;
    }
    return sqrtf(lenSq);
}

void normalize(vec2 *v) {
    f32 lenSq = v->x * v->x + v->y * v->y;
    if(lenSq < EPSILON) {
        return;
    }
    f32 invLen = 1.0f / sqrtf(lenSq);
    v->x *= invLen;
    v->y *= invLen;
}

vec2 normalized(vec2 v) {
    f32 lenSq = v.x * v.x + v.y * v.y;
    if(lenSq < EPSILON) {
        return v;
    }
    f32 invLen = 1.0f / sqrtf(lenSq);
    vec2 result = { v.x * invLen, v.y * invLen };
    return result;
}

f32 angle(vec2 l, vec2 r) {
    f32 lenSqL = l.x * l.x + l.y * l.y;
    f32 lenSqR = r.x * r.x + r.y * r.y;
    if(lenSqL < EPSILON || lenSqR < EPSILON) {
        return 0;
    }
    f32 dot = l.x * r.x + l.y * r.y;
    f32 len = sqrtf(lenSqL) * sqrtf(lenSqR);
    return acosf(dot / len);
}

vec2 project(vec2 a, vec2 b) {
    f32 magB = len(b);
    if(magB < EPSILON) {
        vec2 zero = {};
        return zero;
    }
    f32 scale = dot(a, b) / magB;
    return b * scale;
}

vec2 reject(vec2 a, vec2 b) {
    vec2 projection = project(a, b);
    return a - projection;
}

vec2 reflect(vec2 a, vec2 b) {
    f32 magB = len(b);
    if(magB < EPSILON) {
        vec2 zero = {};
        return zero;
    }
    
    f32 scale = dot(a, b) / magB;
    vec2 proj2 = b * (scale * 2.0f);
    return a - proj2;
}

vec2 lerp(vec2 a, vec2 b, f32 t) {
    vec2 result = {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t
    };
    return result;
}

vec2 nlerp(vec2 a, vec2 b, f32 t) {
    vec2 result = {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t
    };
    return normalized(result);
}

vec2 slerp(vec2 s, vec2 e, f32 t) {
    if(t < 0.01f) {
        return lerp(s, e, t);
    }
    vec2 from = normalized(s);
    vec2 to = normalized(e);
    f32 theta = angle(from, to);
    f32 sin_theta = sinf(theta);
    f32 a = sinf((1.0f - t) * theta) / sin_theta;
    f32 b = sinf(t * theta) / sin_theta;
    return from * a + to * b;
}

// VEC3 FUNCTIONS
vec3 operator+(vec3 l, vec3 r) {
    vec3 result = { l.x + r.x, l.y + r.y, l.z + r.z };
    return result;
}

vec3 operator-(vec3 l, vec3 r) {
    vec3 result = { l.x - r.x, l.y - r.y, l.z - r.z };
    return result;
}

vec3 operator*(vec3 l, vec3 r) {
    vec3 result = { l.x * r.x, l.y * r.y, l.z * r.z };
    return result;
}

vec3 operator*(vec3 v, f32 f) {
    vec3 result = { v.x * f, v.y * f, v.z * f };
    return result;
}

vec3 operator/(vec3 l, vec3 r) {
    vec3 result = { l.x / r.x, l.y / r.y, l.z / r.z };
    return result;
}

f32 dot(vec3 l, vec3 r) {
    return l.x * r.x + l.y * r.y + l.z * r.z;
}

f32 lenSq(vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

f32 len(vec3 v) {
    f32 lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
    if(lenSq < EPSILON) {
        return 0.0f;
    }
    return sqrtf(lenSq);
}

void normalize(vec3 *v) {
    f32 lenSq = v->x * v->x + v->y * v->y + v->z * v->z;
    if(lenSq < EPSILON) {
        return;
    }
    f32 invLen = 1.0f / sqrtf(lenSq);
    v->x *= invLen;
    v->y *= invLen;
    v->z *= invLen;
}

vec3 normalized(vec3 v) {
    f32 lenSq = v.x * v.x + v.y * v.y + v.z * v.z;;
    if(lenSq < EPSILON) {
        return v;
    }
    f32 invLen = 1.0f / sqrtf(lenSq);
    vec3 result = { v.x * invLen, v.y * invLen, v.z * invLen };
    return result;
}

f32 angle(vec3 l, vec3 r) {
    f32 lenSqL = l.x * l.x + l.y * l.y + l.z * l.z;
    f32 lenSqR = r.x * r.x + r.y * r.y + r.z * r.z;
    if(lenSqL < EPSILON || lenSqR < EPSILON) {
        return 0;
    }
    f32 dot = l.x * r.x + l.y * r.y + l.z * r.z;
    f32 len = sqrtf(lenSqL) * sqrtf(lenSqR);
    return acosf(dot / len);
}

vec3 project(vec3 a, vec3 b) {
    f32 magB = len(b);
    if(magB < EPSILON) {
        vec3 zero = {};
        return zero;
    }
    f32 scale = dot(a, b) / magB;
    return b * scale;
}

vec3 reject(vec3 a, vec3 b) {
    vec3 projection = project(a, b);
    return a - projection;
}

vec3 reflect(vec3 a, vec3 b) {
    f32 magB = len(b);
    if(magB < EPSILON) {
        vec3 zero = {};
        return zero;
    }
    
    f32 scale = dot(a, b) / magB;
    vec3 proj2 = b * (scale * 2.0f);
    return a - proj2;
}

vec3 cross(vec3 l, vec3 r) {
    vec3 result = {
        l.y * r.z - l.z * r.y,      
        l.z * r.x - l.x * r.z,      
        l.x * r.y - l.y * r.x
    };
    return result;
}

vec3 lerp(vec3 a, vec3 b, f32 t) {
    vec3 result = {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t
    };
    return result;
}

vec3 nlerp(vec3 a, vec3 b, f32 t) {
    vec3 result = {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t
    };
    return normalized(result);
}

vec3 slerp(vec3 s, vec3 e, f32 t) {
    if(t < 0.01f) {
        return lerp(s, e, t);
    }
    vec3 from = normalized(s);
    vec3 to = normalized(e);
    f32 theta = angle(from, to);
    f32 sin_theta = sinf(theta);
    f32 a = sinf((1.0f - t) * theta) / sin_theta;
    f32 b = sinf(t * theta) / sin_theta;
    return from * a + to * b;
}

// MAT2 FUNCTIONS
mat2 Mat2Identity() {
    mat2 result = {
        1, 0,
        0, 1
    };
    return result;
}

void Mat2Print(mat2 m) {
    char buffer[256];
    sprintf(buffer, "|%.1f  %.1f|\n", m.m00, m.m01);
    OutputDebugString(buffer);
    sprintf(buffer, "|%.1f  %.1f|\n", m.m10, m.m11);
    OutputDebugString(buffer);
    OutputDebugString("##########################################\n");
}

mat2 Mat2Rotate(f32 angle) {
    mat2 result = {
        cosf(angle), sinf(angle),
        -sinf(angle), cosf(angle),
    };
    return result;
}

mat2 operator+(mat2 a, mat2 b) {
    mat2 result = {
        a.m00 + b.m00, a.m01 + b.m01,
        a.m10 + b.m10, a.m11 + b.m11
    };
    return result;
}

mat2 operator*(mat2 m, f32 f) {
    mat2 result = {
        m.m00 * f, m.m01 * f,
        m.m10 * f, m.m11 * f
    };
    return result;
}

vec2 operator*(mat2 m, vec2 v) {
    vec2 result = {
        m.m00 * v.x + m.m01 * v.y,
        m.m10 * v.x + m.m11 * v.y
    };
    return  result;
}

mat2 operator*(mat2 a, mat2 b) {
    mat2 result = {
        a.m00 * b.m00 + a.m01 * b.m10,
        a.m00 * b.m01 + a.m01 * b.m11,
        a.m10 * b.m00 + a.m10 * b.m10,
        a.m10 * b.m01 + a.m11 * b.m11
    };
    return result;
}

// MAT3 FUNCTIONS
mat3 Mat3Identity() {
    mat3 result = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };
    return result;
}

void Mat3Print(mat3 m) {
    char buffer[256];
    sprintf(buffer, "|%.1f  %.1f  %.1f|\n", m.m00, m.m01, m.m02);
    OutputDebugString(buffer);
    sprintf(buffer, "|%.1f  %.1f  %.1f|\n", m.m10, m.m11, m.m12);
    OutputDebugString(buffer);
    sprintf(buffer, "|%.1f  %.1f  %.1f|\n", m.m20, m.m21, m.m22);
    OutputDebugString(buffer);
    OutputDebugString("##########################################\n");
}

mat3 Mat3Scale(f32 x, f32 y, f32 z) {
    mat3 result = {
        x, 0, 0,
        0, y, 0,
        0, 0, z
    };
    return result;
}

mat3 Mat3RotateX(f32 angle) {
    mat3 result = {
        1, 0, 0,
        0, cosf(angle), sinf(angle),
        0, -sinf(angle), cosf(angle), 
    };
    return result;
}

mat3 Mat3RotateY(f32 angle) {
    mat3 result = {
        cosf(angle), 0, -sinf(angle),
        0, 1, 0,
        sinf(angle), 0, cosf(angle),
    };
    return result;
}

mat3 Mat3RotateZ(f32 angle) {
    mat3 result = {
        cosf(angle), sinf(angle), 0,
        -sinf(angle), cosf(angle), 0,
        0, 0, 1,
    };
    return result;
}


mat3 operator+(mat3 a, mat3 b) {
    mat3 result = {
        a.m00 + b.m00, a.m01 + b.m01, a.m02 + b.m02, 
        a.m10 + b.m10, a.m11 + b.m11, a.m12 + b.m12, 
        a.m20 + b.m20, a.m21 + b.m21, a.m22 + b.m22
    };
    return result;
}

mat3 operator*(mat3 m, f32 f) {
    mat3 result = {
        m.m00 * f, m.m01 * f, m.m02 * f, 
        m.m10 * f, m.m11 * f, m.m12 * f, 
        m.m20 * f, m.m21 * f, m.m22 * f
    };
    return result;
}

vec3 operator*(mat3 m, vec3 v) {
    vec3 result = {
        m.m00 * v.x + m.m01 * v.y + m.m02 * v.z, 
        m.m10 * v.x + m.m11 * v.y + m.m12 * v.z, 
        m.m20 * v.x + m.m21 * v.y + m.m22 * v.z
    };
    return result;
}

mat3 operator*(mat3 a, mat3 b) {
    mat3 result = {
        a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20,
        a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21, 
        a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22, 
        a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20,
        a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21, 
        a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22, 
        a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20,
        a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21, 
        a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22
    };
    return result;
}

// MAT4 FUNCTIONS
mat4 Mat4Identity() {
    mat4 result = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return result;
}

void Mat4Print(mat4 m) {
    char buffer[256];
    sprintf(buffer, "|%.1f  %.1f  %.1f  %.1f|\n", m.m00, m.m01, m.m02, m.m03);
    OutputDebugString(buffer);
    sprintf(buffer, "|%.1f  %.1f  %.1f  %.1f|\n", m.m10, m.m11, m.m12, m.m13);
    OutputDebugString(buffer);
    sprintf(buffer, "|%.1f  %.1f  %.1f  %.1f|\n", m.m20, m.m21, m.m22, m.m23);
    OutputDebugString(buffer);
    sprintf(buffer, "|%.1f  %.1f  %.1f  %.1f|\n", m.m30, m.m31, m.m32, m.m33);
    OutputDebugString(buffer);
    OutputDebugString("##########################################\n");
}

mat4 Mat4Translate(f32 x, f32 y, f32 z) {
    mat4 result = {
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
    };
    return result;
}

mat4 Mat4Scale(f32 x, f32 y, f32 z) {
    mat4 result = {
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    };
    return result;
}

mat4 Mat4RotateX(f32 angle) {
    mat4 result = {
        1, 0, 0, 0,
        0, cosf(angle), sinf(angle), 0,
        0, -sinf(angle), cosf(angle), 0,
        0, 0, 0, 1 
    };
    return result;
}

mat4 Mat4RotateY(f32 angle) {
    mat4 result = {
        cosf(angle), 0, -sinf(angle), 0,
        0, 1, 0, 0,
        sinf(angle), 0, cosf(angle), 0,
        0, 0, 0, 1 
    };
    return result;
}

mat4 Mat4RotateZ(f32 angle) {
    mat4 result = {
        cosf(angle), sinf(angle), 0, 0,
        -sinf(angle), cosf(angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return result;
}

mat4 Mat4Transpose(mat4 m) {
    mat4 result = {
        m.m00, m.m10, m.m20, m.m30, 
        m.m01, m.m11, m.m21, m.m31, 
        m.m02, m.m12, m.m22, m.m32, 
        m.m03, m.m13, m.m23, m.m33 
    };
    return result;
}

mat4 operator+(mat4 a, mat4 b) {
    mat4 result = {
        a.m00 + b.m00, a.m01 + b.m01, a.m02 + b.m02, a.m03 + b.m03,
        a.m10 + b.m10, a.m11 + b.m11, a.m12 + b.m12, a.m13 + b.m13,
        a.m20 + b.m20, a.m21 + b.m21, a.m22 + b.m22, a.m23 + b.m23,
        a.m30 + b.m30, a.m31 + b.m31, a.m32 + b.m32, a.m33 + b.m33
    };
    return result;
}

mat4 operator*(mat4 m, f32 f) {
    mat4 result = {
        m.m00 * f, m.m01 * f, m.m02 * f, m.m03 * f,
        m.m10 * f, m.m11 * f, m.m12 * f, m.m13 * f,
        m.m20 * f, m.m21 * f, m.m22 * f, m.m23 * f,
        m.m30 * f, m.m31 * f, m.m32 * f, m.m33 * f
    };
    return result;
}

internal
__m128 Vec4Mat4MulSSE(__m128 v, mat4 m) {
    // first transpose v
    __m128 vX = _mm_shuffle_ps(v, v, 0x00); // (vx,vx,vx,vx)
    __m128 vY = _mm_shuffle_ps(v, v, 0x55); // (vy,vy,vy,vy)
    __m128 vZ = _mm_shuffle_ps(v, v, 0xAA); // (vz,vz,vz,vz)
    __m128 vW = _mm_shuffle_ps(v, v, 0xFF); // (vw,vw,vw,vw)
    _MM_TRANSPOSE4_PS(m.row[0], m.row[1], m.row[2], m.row[3]);
    __m128 result = _mm_mul_ps(vX, m.row[0]);
    result = _mm_add_ps(result, _mm_mul_ps(vY, m.row[1]));
    result = _mm_add_ps(result, _mm_mul_ps(vZ, m.row[2]));
    result = _mm_add_ps(result, _mm_mul_ps(vW, m.row[3]));
    return result;
}

mat4 Mat4MulSSE(mat4 a, mat4 b) {
    mat4 result;
    result.row[0] = Vec4Mat4MulSSE(a.row[0], b);
    result.row[1] = Vec4Mat4MulSSE(a.row[1], b);
    result.row[2] = Vec4Mat4MulSSE(a.row[2], b);
    result.row[3] = Vec4Mat4MulSSE(a.row[3], b);
    return result;
}

#if 0

vec4 operator*(mat4 m, vec4 v) {
    vec4 result = {
        m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w, 
        m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w, 
        m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w, 
        m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w 
    };
    return result;
}

mat4 operator*(mat4 a, mat4 b) {
    mat4 result = {
        a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20 + a.m03 * b.m30, 
        a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21 + a.m03 * b.m31, 
        a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22 + a.m03 * b.m32,
        a.m00 * b.m03 + a.m01 * b.m13 + a.m02 * b.m23 + a.m03 * b.m33,
        a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20 + a.m13 * b.m30, 
        a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31, 
        a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32,
        a.m10 * b.m03 + a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33,
        a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20 + a.m23 * b.m30, 
        a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31, 
        a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32,
        a.m20 * b.m03 + a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33,
        a.m30 * b.m00 + a.m31 * b.m10 + a.m32 * b.m20 + a.m33 * b.m30, 
        a.m30 * b.m01 + a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31, 
        a.m30 * b.m02 + a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32,
        a.m30 * b.m03 + a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33
    };
    return result;
}
#else

vec4 operator*(mat4 m, vec4 v) {
    vec4 result;
    __m128 vX = _mm_shuffle_ps(v.wide, v.wide, 0x00); // (vx,vx,vx,vx)
    __m128 vY = _mm_shuffle_ps(v.wide, v.wide, 0x55); // (vy,vy,vy,vy)
    __m128 vZ = _mm_shuffle_ps(v.wide, v.wide, 0xAA); // (vz,vz,vz,vz)
    __m128 vW = _mm_shuffle_ps(v.wide, v.wide, 0xFF); // (vw,vw,vw,vw)
    _MM_TRANSPOSE4_PS(m.row[0], m.row[1], m.row[2], m.row[3]);
    result.wide = _mm_mul_ps(vX, m.row[0]);
    result.wide = _mm_add_ps(result.wide, _mm_mul_ps(vY, m.row[1]));
    result.wide = _mm_add_ps(result.wide, _mm_mul_ps(vZ, m.row[2]));
    result.wide = _mm_add_ps(result.wide, _mm_mul_ps(vW, m.row[3]));
    return result;
}



mat4 operator*(mat4 a, mat4 b) {
    mat4 result;
    result.row[0] = Vec4Mat4MulSSE(a.row[0], b);
    result.row[1] = Vec4Mat4MulSSE(a.row[1], b);
    result.row[2] = Vec4Mat4MulSSE(a.row[2], b);
    result.row[3] = Vec4Mat4MulSSE(a.row[3], b);
    return result;
}

#endif

mat4 Mat4Frustum(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f) {	
    mat4 result = {
        (2.0f * n) / (r - l), 0, -(r + l) / (r - l), 0,
        0, (2.0f * n) / (t - b), -(t + b) / (t - b), 0,
        0, 0, f / (f - n), -(f * n) / (f - n),
        0, 0, 1, 0
    };
    return result;
}

mat4 Mat4Perspective(f32 fov, f32 aspect, f32 znear, f32 zfar) {
	f32 ymax = znear * tanf(fov * 3.14159265359f / 360.0f);
	f32 xmax = ymax * aspect;
	return Mat4Frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

mat4 Mat4Ortho(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f) {
    mat4 result = {
        2.0f / (r - l), 0, 0, -(r + l) / (r - l),
        0, 2.0f / (t - b), 0, -(t + b) / (t - b),
        0, 0, 1.0f / (f - n), -n / (f - n),
        0, 0, 0, 1
    };
    return result;
}

mat4 Mat4LookAt(vec3 position, vec3 target, vec3 up) {
    vec3 zaxis = normalized(target - position);
    vec3 xaxis = normalized(cross(up, zaxis));
    vec3 yaxis = cross(zaxis, xaxis);
    mat4 result = {
        xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, position), 
        yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, position), 
        zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, position),
        0,       0,       0,       1 
    };
    return result;
}


vec3 Vec4ToVec3(vec4 v) {
    vec3 result = { v.x, v.y, v.z };
    return result;
}

vec4 Vec3ToVec4(vec3 v, f32 w) {
    vec4 result = {v.x, v.y, v.z, w};
    return result;
}

rectangle2i RectangleInvertedInfinity() {
    rectangle2i result;
    result.minX = result.minY = INT_MAX;
    result.maxX = result.maxY = -INT_MAX;
    return result;
}

rectangle2i RectangleIntersect(rectangle2i a, rectangle2i b) {
    rectangle2i result;
    result.minX = (a.minX > b.minX) ? a.minX : b.minX;
    result.minY = (a.minY > b.minY) ? a.minY : b.minY;
    result.maxX = (a.maxX < b.maxX) ? a.maxX : b.maxX;
    result.maxY = (a.maxY < b.maxY) ? a.maxY : b.maxY;
    return result;
}

rectangle2i RectangleUnion(rectangle2i a, rectangle2i b) {
    rectangle2i result;
    result.minX = (a.minX < b.minX) ? a.minX : b.minX;
    result.minY = (a.minY < b.minY) ? a.minY : b.minY;
    result.maxX = (a.maxX > b.maxX) ? a.maxX : b.maxX;
    result.maxY = (a.maxY > b.maxY) ? a.maxY : b.maxY;
    return result;
}

i32 RectangleGetClampArea(rectangle2i a) {
    i32 width = (a.maxX - a.minX);
    i32 height = (a.maxY - a.minY);
    i32 result = 0;
    if((width > 0) && (height) > 0) {
        result = width * height;
    }
    return result;
}

bool RectangleHasArea(rectangle2i a) {
    bool result = ((a.minX < a.maxX) && (a.minY < a.maxY));
    return result;
}

