#include <windows.h>
#include "lh_renderer.h"
#include "lh_platform.h"
#include "lh_texture.h"
#include <math.h>
#include <immintrin.h>
#include <xmmintrin.h>

// TODO: implement the new trianglel rasterizer and SIMD omptimized it.

#define MAX_VERTICES_PER_CLIPPED_TRIANGLE 16
#define Align16(value) ((value + 15) & ~15)

struct RenderWork {
    Vertex *vertices;
    i32 verticesCount;
    u32 *indices;
    i32 indicesCount;
    Texture *bitmap;
    vec3 lightDir;
    mat4 world;
};

struct Renderer {
    HBITMAP handle;
    HDC hdc;
    u32 *colorBuffer;
    f32 *depthBuffer;
    i32 bufferWidth;
    i32 bufferHeight;
    mat4 view;
    mat4 proj;
    RenderWork *workArray;
    i32 workCount;
};

struct ThreadParam {
    rectangle2i clipRect;
};

extern Window gWindow;
global_variable Renderer gRenderer;

inline internal
void SwapPoint(Point *a, Point *b) {
    Point tmp = *a;
    *a = *b;
    *b = tmp;
}

inline internal
void SwapVec2(vec2 *a, vec2 *b) {
    vec2 tmp = *a;
    *a = *b;
    *b = tmp;
}

inline internal
void SwapVec3(vec3 *a, vec3 *b) {
    vec3 tmp = *a;
    *a = *b;
    *b = tmp;
}

inline internal 
void SwapInt(i32 *a, i32 *b) {
    i32 tmp = *a;
    *a = *b;
    *b = tmp;
}

internal
f32 clamp(f32 value, f32 min, f32 max) {
    if(value <= min) return min;
    if(value >= max) return max;
    return value;
}

internal
f32 maxFloat(f32 a, f32 b) {
    if(a <= b) return b;
    return a;
}

internal
f32 minFloat(f32 a, f32 b) {
    if(a >= b) return b;
    return a;
}

internal
vec3 SolveBarycentric(vec2 a, vec2 b, vec2 c, vec2 p) {
    vec2 v0 = b - a;
    vec2 v1 = c - a;
    vec2 v2 = p - a;
    f32 d00 = dot(v0, v0);
    f32 d10 = dot(v1, v0);
    f32 d11 = dot(v1, v1);
    f32 d20 = dot(v2, v0);
    f32 d21 = dot(v2, v1);
    f32 denom = d00 * d11 - d10 * d10;
    vec3 result; 
    result.y = (d20 * d11 - d10 * d21) / denom;
    result.z = (d00 * d21 - d20 * d10) / denom;
    result.x = 1.0f - result.y - result.z;
    return result;
}

internal 
void DrawLine(Point a, Point b, u32 color) {
    i32 xDelta = (i32)(b.x - a.x);
    i32 yDelta = (i32)(b.y - a.y);
    i32 sideLength = abs(xDelta) >= abs(yDelta) ? abs(xDelta) : abs(yDelta);
    f32 xInc = (f32)xDelta / (f32)sideLength;
    f32 yInc = (f32)yDelta / (f32)sideLength;
    f32 x = a.x;
    f32 y = a.y;
    for(i32 i = 0; i <= sideLength; ++i) {
        
        vec2 start = {a.x, a.y};
        vec2 delta = { b.x - a.x, b.y - a.y };
        vec2 p = { x, y };
        vec2 pRel = start - p;
        f32 t = len(pRel) / len(delta);
        f32 interpolatedReciprocalZ = ((1.0f/a.z) + ((1.0f/b.z) - (1.0f/a.z)) * t); 
        if(x >= 0 && x < gRenderer.bufferWidth && y >= 0 && y < gRenderer.bufferHeight) {
            if(interpolatedReciprocalZ >= gRenderer.depthBuffer[(i32)y * gRenderer.bufferWidth + (i32)x]) {
                gRenderer.depthBuffer[(i32)y * gRenderer.bufferWidth + (i32)x] = interpolatedReciprocalZ;
                gRenderer.colorBuffer[(i32)y * gRenderer.bufferWidth + (i32)x] = color;
            } 
        }
        x += xInc;
        y += yInc;
    }
}

internal
void DrawLineTriangle(Point a, Point b, Point c, u32 color) {
    DrawLine(a, b, color);
    DrawLine(b, c, color);
    DrawLine(c, a, color);
}

internal
f32 ORIENTED2D(Point a, Point b, Point c) {
    f32 result = ((a.x - c.x) * (b.y - c.y)) - ((a.y - c.y) * (b.x - c.x));
    return result;
}

internal
void TriangleRasterizer(Point a, Point b, Point c, vec2 aUv, vec2 bUv, vec2 cUv, vec3 aNorm, vec3 bNorm, vec3 cNorm,
                        vec3 aFragPos, vec3 bFragPos, vec3 cFragPos, Texture *bitmap, vec3 lightDir,
                        rectangle2i clipRect) {

    ASSERT(((uintptr_t)gRenderer.colorBuffer & 15) == 0);
    // compute trinangle AABB
    rectangle2i fillRect;
    fillRect.minX = minFloat(minFloat(a.x, b.x), c.x);
    fillRect.minY = minFloat(minFloat(a.y, b.y), c.y);
    fillRect.maxX = maxFloat(maxFloat(a.x, b.x), c.x);
    fillRect.maxY = maxFloat(maxFloat(a.y, b.y), c.y);
    // clamp to the tile 
    fillRect = RectangleIntersect(fillRect, clipRect);

    if(RectangleHasArea(fillRect)) {
        // TODO: aline the fill rectangle to 16 bytes boundary
        __m128i startClipMask = _mm_set1_epi8(-1);
        __m128i endClipMask = _mm_set1_epi8(-1);
        __m128i startClipMasks[] = {
            _mm_slli_si128(startClipMask, 0*4),
            _mm_slli_si128(startClipMask, 1*4),
            _mm_slli_si128(startClipMask, 2*4),
            _mm_slli_si128(startClipMask, 3*4),
        };
        __m128i endClipMasks[] = {
            _mm_srli_si128(endClipMask, 3*4),
            _mm_srli_si128(endClipMask, 2*4),
            _mm_srli_si128(endClipMask, 1*4),
            _mm_srli_si128(endClipMask, 0*4),
        };

        if(fillRect.minX & 3) {
            startClipMask = startClipMasks[fillRect.minX & 3];
            fillRect.minX = fillRect.minX & ~3;
        }
        if(fillRect.maxX & 3) {
            endClipMask = endClipMasks[fillRect.maxX & 3];
            fillRect.maxX = (fillRect.maxX & ~3) + 4;
        }

        // modify this function to go by 4 pixel at a time...
        __m128 aPointX = _mm_set1_ps(a.x);
        __m128 aPointY = _mm_set1_ps(a.y);
        __m128 aPointZ = _mm_set1_ps(a.z);
        __m128 bPointX = _mm_set1_ps(b.x);
        __m128 bPointY = _mm_set1_ps(b.y);
        __m128 bPointZ = _mm_set1_ps(b.z);
        __m128 cPointX = _mm_set1_ps(c.x);
        __m128 cPointY = _mm_set1_ps(c.y);
        __m128 cPointZ = _mm_set1_ps(c.z);
        __m128 aUvX = _mm_set1_ps(aUv.x);
        __m128 aUvY = _mm_set1_ps(aUv.y);
        __m128 bUvX = _mm_set1_ps(bUv.x);
        __m128 bUvY = _mm_set1_ps(bUv.y);
        __m128 cUvX = _mm_set1_ps(cUv.x);
        __m128 cUvY = _mm_set1_ps(cUv.y);
        __m128 aNormX = _mm_set1_ps(aNorm.x);
        __m128 aNormY = _mm_set1_ps(aNorm.y);
        __m128 aNormZ = _mm_set1_ps(aNorm.z);
        __m128 bNormX = _mm_set1_ps(bNorm.x);
        __m128 bNormY = _mm_set1_ps(bNorm.y);
        __m128 bNormZ = _mm_set1_ps(bNorm.z);
        __m128 cNormX = _mm_set1_ps(cNorm.x);
        __m128 cNormY = _mm_set1_ps(cNorm.y);
        __m128 cNormZ = _mm_set1_ps(cNorm.z);
        __m128 aFragPosX = _mm_set1_ps(aFragPos.x);
        __m128 aFragPosY = _mm_set1_ps(aFragPos.y);
        __m128 aFragPosZ = _mm_set1_ps(aFragPos.z);
        __m128 bFragPosX = _mm_set1_ps(bFragPos.x);
        __m128 bFragPosY = _mm_set1_ps(bFragPos.y);
        __m128 bFragPosZ = _mm_set1_ps(bFragPos.z);
        __m128 cFragPosX = _mm_set1_ps(cFragPos.x);
        __m128 cFragPosY = _mm_set1_ps(cFragPos.y);
        __m128 cFragPosZ = _mm_set1_ps(cFragPos.z);

        __m128 bitmapWidth = _mm_set1_ps((f32)bitmap->width - 1);
        __m128 bitmapHeight = _mm_set1_ps((f32)bitmap->height - 1);
        __m128 zero = _mm_set1_ps(0.0f);
        __m128 one = _mm_set1_ps(1.0f);
        __m128 two = _mm_set1_ps(2.0f);
        __m128 m255 = _mm_set1_ps(255.0f);
        __m128 minusOne = _mm_set1_ps(-1.0f);
        __m128i maskFF = _mm_set1_epi32(0xFF);

        __m128 v0x = _mm_sub_ps(bPointX, aPointX);
        __m128 v0y = _mm_sub_ps(bPointY, aPointY);
        __m128 v1x = _mm_sub_ps(cPointX, aPointX);
        __m128 v1y = _mm_sub_ps(cPointY, aPointY);
        __m128 d00 = _mm_add_ps(_mm_mul_ps(v0x, v0x), _mm_mul_ps(v0y, v0y));
        __m128 d10 = _mm_add_ps(_mm_mul_ps(v1x, v0x), _mm_mul_ps(v1y, v0y));
        __m128 d11 = _mm_add_ps(_mm_mul_ps(v1x, v1x), _mm_mul_ps(v1y, v1y));
        __m128 denom = _mm_sub_ps(_mm_mul_ps(d00, d11), _mm_mul_ps(d10, d10));


        // vec3 viewPos = {0, -3, -8};
        __m128 viewPosX = _mm_set1_ps(0);
        __m128 viewPosY = _mm_set1_ps(0);
        __m128 viewPosZ = _mm_set1_ps(-5);

        //vec3 lightPos = {3, -3.5f, -4};
        __m128 lightPosX = _mm_set1_ps(3);
        __m128 lightPosY = _mm_set1_ps(0.5f);
        __m128 lightPosZ = _mm_set1_ps(-8);

        //vec3 lightColor = {1, 1, 1};
        __m128 lightColorX = _mm_set1_ps(1);
        __m128 lightColorY = _mm_set1_ps(1);
        __m128 lightColorZ = _mm_set1_ps(1);

        __m128 diffuseColorX = _mm_set1_ps(0.5f);
        __m128 diffuseColorY = _mm_set1_ps(1);
        __m128 diffuseColorZ = _mm_set1_ps(0.5f);
        
       //vec3 lightColor = {1, 1, 1};
        __m128 specularColorX = _mm_set1_ps(0);
        __m128 specularColorY = _mm_set1_ps(1);
        __m128 specularColorZ = _mm_set1_ps(0);
        
        __m128 specComponent = _mm_set1_ps(2.0f);

        __m128 ambientStrength  = _mm_set1_ps(0.3f);
        __m128 specularStrength = _mm_set1_ps(0.8f);
        __m128 diffuseStrength = _mm_set1_ps(0.6f);


        i32 minX = fillRect.minX;
        i32 minY = fillRect.minY;
        i32 maxX = fillRect.maxX;
        i32 maxY = fillRect.maxY;

        for(i32 y = minY; y <= maxY; ++y) {
            __m128 pixelsToTestY = _mm_set1_ps(y);
            __m128i clipMask = startClipMask;
            for(i32 x = minX; x <= maxX; x += 4) {

                // get the old data for the maskout
                u32 *pixelPt = gRenderer.colorBuffer + ((y * gRenderer.bufferWidth) + x);
                __m128i originalDest = _mm_load_si128((__m128i *)pixelPt);
                
                f32 *depthPt = gRenderer.depthBuffer + ((y * gRenderer.bufferWidth) + x);
                __m128 depth = _mm_load_ps(depthPt);
                __m128 pixelsToTestX = _mm_set_ps(x + 3, x + 2, x + 1, x);
                // TODO: standarize the conditional once we load 3d models
                // ORIENTED2D SSE2 version
#if 1
                // CLOCK WIASE TRIALGE TEST
                __m128 insideTriangleBA = _mm_sub_ps(_mm_mul_ps(_mm_sub_ps(bPointX, pixelsToTestX), _mm_sub_ps(aPointY, pixelsToTestY)),
                                                     _mm_mul_ps(_mm_sub_ps(bPointY, pixelsToTestY), _mm_sub_ps(aPointX, pixelsToTestX)));
                __m128 insideTriangleAC = _mm_sub_ps(_mm_mul_ps(_mm_sub_ps(aPointX, pixelsToTestX), _mm_sub_ps(cPointY, pixelsToTestY)),
                                                     _mm_mul_ps(_mm_sub_ps(aPointY, pixelsToTestY), _mm_sub_ps(cPointX, pixelsToTestX)));
                __m128 insideTriangleCB = _mm_sub_ps(_mm_mul_ps(_mm_sub_ps(cPointX, pixelsToTestX), _mm_sub_ps(bPointY, pixelsToTestY)),
                                                     _mm_mul_ps(_mm_sub_ps(cPointY, pixelsToTestY), _mm_sub_ps(bPointX, pixelsToTestX)));
#else
                // COUNTER CLOCK WISE TRIANGLE TEST
                __m128 insideTriangleBA = _mm_sub_ps(_mm_mul_ps(_mm_sub_ps(aPointX, pixelsToTestX), _mm_sub_ps(bPointY, pixelsToTestY)),
                                                     _mm_mul_ps(_mm_sub_ps(aPointY, pixelsToTestY), _mm_sub_ps(bPointX, pixelsToTestX)));
                __m128 insideTriangleAC = _mm_sub_ps(_mm_mul_ps(_mm_sub_ps(bPointX, pixelsToTestX), _mm_sub_ps(cPointY, pixelsToTestY)),
                                                     _mm_mul_ps(_mm_sub_ps(bPointY, pixelsToTestY), _mm_sub_ps(cPointX, pixelsToTestX)));
                __m128 insideTriangleCB = _mm_sub_ps(_mm_mul_ps(_mm_sub_ps(cPointX, pixelsToTestX), _mm_sub_ps(aPointY, pixelsToTestY)),
                                                     _mm_mul_ps(_mm_sub_ps(cPointY, pixelsToTestY), _mm_sub_ps(aPointX, pixelsToTestX)));
#endif

                // mask that tell me if the pixel is inside the triangle to render
                __m128 writeMask = _mm_and_ps(_mm_and_ps(_mm_cmpge_ps(insideTriangleBA, zero), _mm_cmpge_ps(insideTriangleAC, zero)), _mm_cmpge_ps(insideTriangleCB, zero));
                if(_mm_movemask_ps(writeMask)) {
                    __m128i writeMaski = _mm_castps_si128(writeMask);
                    // calculate the SSE2 version of SolveBarycentric...
                    __m128 v2x = _mm_sub_ps(pixelsToTestX, aPointX);
                    __m128 v2y = _mm_sub_ps(pixelsToTestY, aPointY);
                    __m128 d20 = _mm_add_ps(_mm_mul_ps(v2x, v0x), _mm_mul_ps(v2y, v0y));
                    __m128 d21 = _mm_add_ps(_mm_mul_ps(v2x, v1x), _mm_mul_ps(v2y, v1y));
                    __m128 gamma = _mm_div_ps(_mm_sub_ps(_mm_mul_ps(d20, d11), _mm_mul_ps(d10, d21)), denom);
                    __m128 beta =  _mm_div_ps(_mm_sub_ps(_mm_mul_ps(d00, d21), _mm_mul_ps(d20, d10)), denom);
                    __m128 alpha = _mm_sub_ps(one, gamma);
                    alpha = _mm_sub_ps(alpha, beta);

                    __m128 interReciZ = _mm_add_ps(_mm_add_ps(_mm_mul_ps(aPointZ, alpha), _mm_mul_ps(bPointZ, gamma)), _mm_mul_ps(cPointZ, beta));
                    __m128 depthTestMask = _mm_cmpge_ps(interReciZ, depth);
                    if(_mm_movemask_ps(depthTestMask)) {
                        __m128i depthTestMaski = _mm_castps_si128(depthTestMask);
                        // Update the writeMask with the new information
                        writeMaski = _mm_and_si128(writeMaski, depthTestMaski);
                        writeMask = _mm_and_ps(writeMask, depthTestMask);
                        writeMaski = _mm_and_si128(writeMaski, clipMask);
                        writeMask = _mm_and_ps(writeMask, _mm_castsi128_ps(clipMask));
                        
                        __m128 interpolatedU = _mm_div_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_mul_ps(aUvX, aPointZ), alpha), _mm_mul_ps(_mm_mul_ps(bUvX, bPointZ), gamma)), _mm_mul_ps(_mm_mul_ps(cUvX, cPointZ), beta)), interReciZ);
                        __m128 interpolatedV = _mm_div_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_mul_ps(aUvY, aPointZ), alpha), _mm_mul_ps(_mm_mul_ps(bUvY, bPointZ), gamma)), _mm_mul_ps(_mm_mul_ps(cUvY, cPointZ), beta)), interReciZ);

                        // clamp uvs to be 0-1
                        interpolatedU = _mm_min_ps(_mm_max_ps(interpolatedU, zero), one);
                        interpolatedV = _mm_min_ps(_mm_max_ps(interpolatedV, zero), one);
                        __m128i bitmapX = _mm_cvtps_epi32(_mm_mul_ps(interpolatedU, bitmapWidth));
                        __m128i bitmapY = _mm_cvtps_epi32(_mm_mul_ps(interpolatedV, bitmapHeight));

                        // fetch the texture data...
                        __m128i color;
                        for(i32 i = 0; i < 4; ++i) {
                            i32 textureX = Mi(bitmapX, i);
                            i32 textureY = Mi(bitmapY, i);
                            Mi(color, i) = ((u32 *)bitmap->data)[textureY * bitmap->width + textureX];
                        }
#if 1
                        // implement SSE2 version of the Phong Lighting Model.
                        // interpolate the normals and fragment position to get the current normal and fragment.
                        __m128 interpolatedNormalX = _mm_add_ps(_mm_add_ps(_mm_mul_ps(aNormX, alpha), _mm_mul_ps(bNormX, gamma)), _mm_mul_ps(cNormX, beta));
                        __m128 interpolatedNormalY = _mm_add_ps(_mm_add_ps(_mm_mul_ps(aNormY, alpha), _mm_mul_ps(bNormY, gamma)), _mm_mul_ps(cNormY, beta));
                        __m128 interpolatedNormalZ = _mm_add_ps(_mm_add_ps(_mm_mul_ps(aNormZ, alpha), _mm_mul_ps(bNormZ, gamma)), _mm_mul_ps(cNormZ, beta));
                        // normalized the interpolatedNormals...
                        __m128 squaredLength = _mm_add_ps(
                                                    _mm_add_ps(_mm_mul_ps(interpolatedNormalX, interpolatedNormalX),
                                                               _mm_mul_ps(interpolatedNormalY, interpolatedNormalY)),
                                               _mm_mul_ps(interpolatedNormalZ, interpolatedNormalZ));
                        __m128 length = _mm_sqrt_ps(squaredLength);
                        __m128 normalizeInterpolatedNormalX = _mm_div_ps(interpolatedNormalX, length);
                        __m128 normalizeInterpolatedNormalY = _mm_div_ps(interpolatedNormalY, length);
                        __m128 normalizeInterpolatedNormalZ = _mm_div_ps(interpolatedNormalZ, length);

                        __m128 interpolatedFragPosX = _mm_add_ps(_mm_add_ps(_mm_mul_ps(aFragPosX, alpha), _mm_mul_ps(bFragPosX, gamma)), _mm_mul_ps(cFragPosX, beta));
                        __m128 interpolatedFragPosY = _mm_add_ps(_mm_add_ps(_mm_mul_ps(aFragPosY, alpha), _mm_mul_ps(bFragPosY, gamma)), _mm_mul_ps(cFragPosY, beta));
                        __m128 interpolatedFragPosZ = _mm_add_ps(_mm_add_ps(_mm_mul_ps(aFragPosZ, alpha), _mm_mul_ps(bFragPosZ, gamma)), _mm_mul_ps(cFragPosZ, beta)); 
                       
                        // apply the Lighting to the color.
                        // get the frag colors in floating point values
                        __m128 red   = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(color, 16), maskFF));
                        __m128 green = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(color, 8), maskFF));
                        __m128 blue  = _mm_cvtepi32_ps(_mm_and_si128(color, maskFF));

                        __m128 lightDirX = _mm_sub_ps(lightPosX, interpolatedFragPosX);
                        __m128 lightDirY = _mm_sub_ps(lightPosY, interpolatedFragPosY);
                        __m128 lightDirZ = _mm_sub_ps(lightPosZ, interpolatedFragPosZ);

                        __m128 negativeLightDirX = _mm_mul_ps(lightDirX, minusOne);
                        __m128 negativeLightDirY = _mm_mul_ps(lightDirY, minusOne);
                        __m128 negativeLightDirZ = _mm_mul_ps(lightDirZ, minusOne);

                        __m128 viewDirX = _mm_sub_ps(viewPosX, interpolatedFragPosX);
                        __m128 viewDirY = _mm_sub_ps(viewPosY, interpolatedFragPosY);
                        __m128 viewDirZ = _mm_sub_ps(viewPosZ, interpolatedFragPosZ);

                        squaredLength = _mm_add_ps(
                                        _mm_add_ps(_mm_mul_ps(viewDirX, viewDirX),
                                                   _mm_mul_ps(viewDirY, viewDirY)),
                                                   _mm_mul_ps(viewDirZ, viewDirZ));
                        length = _mm_sqrt_ps(squaredLength);
                        viewDirX = _mm_div_ps(viewDirX, length);
                        viewDirY = _mm_div_ps(viewDirY, length);
                        viewDirZ = _mm_div_ps(viewDirZ, length);

                        squaredLength = _mm_add_ps(
                                        _mm_add_ps(_mm_mul_ps(normalizeInterpolatedNormalX, normalizeInterpolatedNormalX),
                                                   _mm_mul_ps(normalizeInterpolatedNormalY, normalizeInterpolatedNormalY)),
                                                   _mm_mul_ps(normalizeInterpolatedNormalZ, normalizeInterpolatedNormalZ));
                        length = _mm_sqrt_ps(squaredLength);
                        
                        __m128 scale = _mm_div_ps(
                                             _mm_add_ps(
                                             _mm_add_ps(_mm_mul_ps(negativeLightDirX, normalizeInterpolatedNormalX),
                                                        _mm_mul_ps(negativeLightDirY, normalizeInterpolatedNormalY)),
                                                        _mm_mul_ps(negativeLightDirZ, normalizeInterpolatedNormalZ)),
                                       length);
                        scale = _mm_mul_ps(scale, two);
                        __m128 proj2X = _mm_mul_ps(normalizeInterpolatedNormalX, scale); 
                        __m128 proj2Y = _mm_mul_ps(normalizeInterpolatedNormalY, scale); 
                        __m128 proj2Z = _mm_mul_ps(normalizeInterpolatedNormalZ, scale);

                        __m128  reflectDirX = _mm_sub_ps(negativeLightDirX, proj2X);
                        __m128  reflectDirY = _mm_sub_ps(negativeLightDirY, proj2Y);
                        __m128  reflectDirZ = _mm_sub_ps(negativeLightDirZ, proj2Z);

                        squaredLength = _mm_add_ps(
                                        _mm_add_ps(_mm_mul_ps(reflectDirX, reflectDirX),
                                                   _mm_mul_ps(reflectDirY, reflectDirY)),
                                                   _mm_mul_ps(reflectDirZ, reflectDirZ));
                        length = _mm_sqrt_ps(squaredLength);
                        reflectDirX = _mm_div_ps(reflectDirX, length);
                        reflectDirY = _mm_div_ps(reflectDirY, length);
                        reflectDirZ = _mm_div_ps(reflectDirZ, length);


                        __m128 ambientX = _mm_mul_ps(lightColorX, ambientStrength);
                        __m128 ambientY = _mm_mul_ps(lightColorY, ambientStrength);
                        __m128 ambientZ = _mm_mul_ps(lightColorZ, ambientStrength);

                        __m128 diff =  _mm_min_ps(_mm_max_ps(
                                        _mm_add_ps(
                                             _mm_add_ps(_mm_mul_ps(normalizeInterpolatedNormalX, lightDirX),
                                                        _mm_mul_ps(normalizeInterpolatedNormalY, lightDirY)),
                                        _mm_mul_ps(normalizeInterpolatedNormalZ, lightDirZ)),
                                       zero), one);

                        __m128 diffuseX = _mm_mul_ps(_mm_mul_ps(diffuseColorX, diff), diffuseStrength);
                        __m128 diffuseY = _mm_mul_ps(_mm_mul_ps(diffuseColorY, diff), diffuseStrength);
                        __m128 diffuseZ = _mm_mul_ps(_mm_mul_ps(diffuseColorZ, diff), diffuseStrength);

                        __m128 dotProduct = _mm_add_ps(
                                            _mm_add_ps(_mm_mul_ps(viewDirX, reflectDirX),
                                                       _mm_mul_ps(viewDirY, reflectDirY)),
                                                       _mm_mul_ps(viewDirZ, reflectDirZ));
#if 0
                        __m128 spec = _mm_pow_ps(_mm_max_ps(dotProduct, zero), specComponent);
#else
                        dotProduct = _mm_max_ps(dotProduct, zero);
                        __m128 spec = _mm_mul_ps(dotProduct, dotProduct);
#endif

                        __m128 specularX = _mm_mul_ps(_mm_mul_ps(specularColorX, spec), specularStrength);
                        __m128 specularY = _mm_mul_ps(_mm_mul_ps(specularColorY, spec), specularStrength);
                        __m128 specularZ = _mm_mul_ps(_mm_mul_ps(specularColorZ, spec), specularStrength);

                        __m128 resultX = _mm_mul_ps(_mm_add_ps(_mm_add_ps(ambientX, diffuseX), specularX), red);
                        __m128 resultY = _mm_mul_ps(_mm_add_ps(_mm_add_ps(ambientY, diffuseY), specularY), green);
                        __m128 resultZ = _mm_mul_ps(_mm_add_ps(_mm_add_ps(ambientZ, diffuseZ), specularZ), blue);

                        // clamp to 0-255 range
                        resultX = _mm_min_ps(_mm_max_ps(resultX, zero), m255);
                        resultY = _mm_min_ps(_mm_max_ps(resultY, zero), m255);
                        resultZ = _mm_min_ps(_mm_max_ps(resultZ, zero), m255);

                        __m128i r = _mm_cvtps_epi32(resultX);
                        __m128i g = _mm_cvtps_epi32(resultY);
                        __m128i b = _mm_cvtps_epi32(resultZ);
                        __m128i a = _mm_cvtps_epi32(m255);

                        __m128i sr = _mm_slli_epi32(r, 16);
                        __m128i sg = _mm_slli_epi32(g, 8);
                        __m128i sb = b;
                        __m128i sa = _mm_slli_epi32(a, 24);
                        
                        color = _mm_or_si128(_mm_or_si128(sr, sg), _mm_or_si128(sb, sa));
#endif

                        __m128i colorMaskedOut = _mm_or_si128(_mm_and_si128(writeMaski, color), _mm_andnot_si128(writeMaski, originalDest));
                        __m128 depthMaskOut = _mm_or_ps(_mm_and_ps(writeMask, interReciZ), _mm_andnot_ps(writeMask, depth));
                        _mm_store_si128((__m128i *)pixelPt, colorMaskedOut);
                        _mm_store_ps(depthPt, depthMaskOut);
                    }
                }
                if((x + 4) >= maxX) {
                    clipMask = endClipMask;
                }
                else {
                    clipMask = _mm_set1_epi8(-1);
                }
            }
        }
    }
}

internal
void HomogenousClipping(vec4 *srcVertives, vec2 * srcUVs, vec3 *srcNormals, vec3 *srcFragPos, i32 srcCount,
                        vec4 *dstVetices, vec2* dstUVs, vec3 *dstNormals, vec3 *dstFragPos, i32 *dstCount,
                        i32 index, f32 sign) {
    // TODO: test using pointers intead...
    *dstCount = 0;
    vec4 prevVert = srcVertives[srcCount - 1];
    vec2 prevUv = srcUVs[srcCount - 1];
    vec3 prevNormal = srcNormals[srcCount - 1];
    vec3 prevFragPos = srcFragPos[srcCount - 1];
    f32 prevComponent = prevVert.v[index] * sign;
    bool prevInside = prevComponent <= prevVert.w;

    for(i32 i = 0; i < srcCount; ++i) {
        vec4 currentVert = srcVertives[i];
        vec2 currentUv = srcUVs[i];
        vec3 currentNormal = srcNormals[i];
        vec3 currentFragPos = srcFragPos[i];
        f32 currentComponent = currentVert.v[index] * sign;
        bool currentInside = currentComponent <= currentVert.w;

        if(currentInside ^ prevInside) {
            f32 t = (prevVert.w - prevComponent) / ((prevVert.w - prevComponent) - (currentVert.w - currentComponent));
            vec4 newVertex = {
                lerp(prevVert.x, currentVert.x, t),  
                lerp(prevVert.y, currentVert.y, t),  
                lerp(prevVert.z, currentVert.z, t),  
                lerp(prevVert.w, currentVert.w, t)  
            };
            vec2 newUvs = {
                lerp(prevUv.x, currentUv.x, t),
                lerp(prevUv.y, currentUv.y, t)
            };
            vec3 newNormal = {
                lerp(prevNormal.x, currentNormal.x, t), 
                lerp(prevNormal.y, currentNormal.y, t), 
                lerp(prevNormal.z, currentNormal.z, t) 
            };
            vec3 newFragPos = {
                lerp(prevFragPos.x, currentFragPos.x, t),
                lerp(prevFragPos.y, currentFragPos.y, t),
                lerp(prevFragPos.z, currentFragPos.z, t)
            };
            dstVetices[*dstCount] = newVertex;
            dstUVs[*dstCount] = newUvs;
            dstNormals[*dstCount] = newNormal;
            dstFragPos[*dstCount] = newFragPos;
            *dstCount = *dstCount + 1;
        }
        if(currentInside) {
            dstVetices[*dstCount] = currentVert;
            dstUVs[*dstCount] = currentUv;
            dstNormals[*dstCount] = currentNormal;
            dstFragPos[*dstCount] = currentFragPos;
            *dstCount = *dstCount + 1;
        }
        prevInside = currentInside;
        prevVert = currentVert;
        prevUv = currentUv;
        prevNormal = currentNormal;
        prevFragPos = currentFragPos;
        prevComponent = currentComponent;
    }
}

internal
void RenderVertexArrayFast(Vertex *vertices, u32 *indices,
                           i32 indicesCount, Texture *bitmap, vec3 lightDir, mat4 world, rectangle2i clipRect) {    
    for(i32 i = 0; i < indicesCount; i += 3) {

        Vertex *aVertex = vertices + indices[i + 0];
        Vertex *bVertex = vertices + indices[i + 1];
        Vertex *cVertex = vertices + indices[i + 2];
        vec3 aTmp = aVertex->position;
        vec3 bTmp = bVertex->position;
        vec3 cTmp = cVertex->position;
        vec2 aUv = aVertex->uv;
        vec2 bUv = bVertex->uv;
        vec2 cUv = cVertex->uv;
        vec3 aNormal = aVertex->normal;
        vec3 bNormal = bVertex->normal;
        vec3 cNormal = cVertex->normal;

        vec4 a = {aTmp.x, aTmp.y, aTmp.z, 1.0f};
        vec4 b = {bTmp.x, bTmp.y, bTmp.z, 1.0f};
        vec4 c = {cTmp.x, cTmp.y, cTmp.z, 1.0f};

        // multiply by the world matrix...
        a = world * a;
        b = world * b;
        c = world * c;
        
        // normals in world space
        aNormal = Vec4ToVec3(world * Vec3ToVec4(aNormal, 0.0f));
        bNormal = Vec4ToVec3(world * Vec3ToVec4(bNormal, 0.0f));
        cNormal = Vec4ToVec3(world * Vec3ToVec4(cNormal, 0.0f));

        vec3 aFragPos = Vec4ToVec3(world * Vec3ToVec4(aTmp, 1.0f));
        vec3 bFragPos = Vec4ToVec3(world * Vec3ToVec4(bTmp, 1.0f));
        vec3 cFragPos = Vec4ToVec3(world * Vec3ToVec4(cTmp, 1.0f));

        // transform the vertices relative to the camera
        mat4 view = gRenderer.view;
        a = view * a;
        b = view * b;
        c = view * c;
        
        // backface culling
        vec3 vecA = Vec4ToVec3(a);
        vec3 ab = Vec4ToVec3(b) - vecA;
        vec3 ac = Vec4ToVec3(c) - vecA;
        // TODO: standarize the cross product once we load 3d models
        vec3 normal = normalized(cross(ab, ac));
        vec3 origin = {0, 0, 0};
        vec3 cameraRay = origin - vecA;
        f32 normalDirection = dot(normal, cameraRay);
        if(normalDirection < 0.0f) {
            continue;
        }

        mat4 proj = gRenderer.proj;
        a = proj * a;
        b = proj * b;
        c = proj * c;

        i32 verticesACount = 3;
        vec4 verticesToClipA[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {a, b, c};
        vec2 uvsToClipA[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {aUv, bUv, cUv};
        vec3 normalsToClipA[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {aNormal, bNormal, cNormal};
        vec3 fragPosToClipA[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {aFragPos, bFragPos, cFragPos};

        i32 verticesBCount = 0;
        vec4 verticesToClipB[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {};
        vec2 uvsToClipB[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {};
        vec3 normalsToClipB[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {};
        vec3 fragPosToClipB[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {};
        HomogenousClipping(verticesToClipA, uvsToClipA, normalsToClipA, fragPosToClipA, verticesACount,
                           verticesToClipB, uvsToClipB, normalsToClipB, fragPosToClipB, &verticesBCount,
                           0, -1.0f);
        HomogenousClipping(verticesToClipB, uvsToClipB, normalsToClipB, fragPosToClipB, verticesBCount,
                           verticesToClipA, uvsToClipA, normalsToClipA, fragPosToClipA, &verticesACount,
                           0, 1.0f);
        HomogenousClipping(verticesToClipA, uvsToClipA, normalsToClipA, fragPosToClipA, verticesACount,
                           verticesToClipB, uvsToClipB, normalsToClipB, fragPosToClipB, &verticesBCount,
                           1, -1.0f);
        HomogenousClipping(verticesToClipB, uvsToClipB, normalsToClipB, fragPosToClipB, verticesBCount,
                           verticesToClipA, uvsToClipA, normalsToClipA, fragPosToClipA, &verticesACount,
                           1, 1.0f);
        HomogenousClipping(verticesToClipA, uvsToClipA, normalsToClipA, fragPosToClipA, verticesACount,
                           verticesToClipB, uvsToClipB, normalsToClipB, fragPosToClipB, &verticesBCount,
                           2, -1.0f);
        HomogenousClipping(verticesToClipB, uvsToClipB, normalsToClipB, fragPosToClipB, verticesBCount,
                           verticesToClipA, uvsToClipA, normalsToClipA, fragPosToClipA, &verticesACount,
                           2, 1.0f);

        for(i32 j = 0; j < verticesACount - 2; ++j) {
            vec4 newA = verticesToClipA[0];
            vec4 newB = verticesToClipA[1 + j];
            vec4 newC = verticesToClipA[2 + j];
            vec2 newUvA = uvsToClipA[0];
            vec2 newUvB = uvsToClipA[1 + j];
            vec2 newUvC = uvsToClipA[2 + j];
            vec3 newNormalA = normalsToClipA[0]; 
            vec3 newNormalB = normalsToClipA[1 + j]; 
            vec3 newNormalC = normalsToClipA[2 + j];
            vec3 newFragPosA = fragPosToClipA[0]; 
            vec3 newFragPosB = fragPosToClipA[1 + j]; 
            vec3 newFragPosC = fragPosToClipA[2 + j]; 
            f32 aInvW = 1.0f/newA.w;
            f32 bInvW = 1.0f/newB.w;
            f32 cInvW = 1.0f/newC.w;
            i32 halfBufferWidth = gRenderer.bufferWidth/2;
            i32 halfBufferHeight = gRenderer.bufferHeight/2;
            Point aPoint = {((newA.x * aInvW) * halfBufferWidth) + halfBufferWidth, ((newA.y * aInvW) * halfBufferHeight) + halfBufferHeight, aInvW};
            Point bPoint = {((newB.x * bInvW) * halfBufferWidth) + halfBufferWidth, ((newB.y * bInvW) * halfBufferHeight) + halfBufferHeight, bInvW};
            Point cPoint = {((newC.x * cInvW) * halfBufferWidth) + halfBufferWidth, ((newC.y * cInvW) * halfBufferHeight) + halfBufferHeight, cInvW};
            TriangleRasterizer(aPoint, bPoint, cPoint,
                               newUvA, newUvB, newUvC,
                               newNormalA, newNormalB, newNormalC,
                               newFragPosA, newFragPosB, newFragPosC,
                               bitmap,
                               lightDir, clipRect);
        }
    }
}

internal
void DoTileRenderWork(void *data) {
    ThreadParam *param = (ThreadParam *)data;
    for(i32 i = 0; i < gRenderer.workCount; ++i) {
        RenderWork *work = gRenderer.workArray + i;
        RenderVertexArrayFast(work->vertices, work->indices,
                              work->indicesCount, work->bitmap, work->lightDir, work->world, param->clipRect);
    }
}

internal
void FlushWorkQueue() {
#if 1
    const i32 tileCountX = 4;
    const i32 tileCountY = 4;
    ThreadParam paramArray[tileCountX*tileCountY];
    i32 tileWidth = gRenderer.bufferWidth / tileCountX;
    i32 tileHeight = gRenderer.bufferHeight / tileCountY;
    tileWidth = ((tileWidth + 3) / 4) * 4;
    i32 paramCount = 0;
    for(i32 tileY = 0; tileY < tileCountY; ++tileY) {
        for(i32 tileX = 0; tileX < tileCountX; ++tileX) {
            ThreadParam *param = paramArray + paramCount++;
            rectangle2i clipRect;
            clipRect.minX = (tileX * tileWidth);
            clipRect.maxX = (clipRect.minX + tileWidth);
            clipRect.minY = (tileY * tileHeight);
            clipRect.maxY = (clipRect.minY + tileHeight);
            if(tileX == (tileCountX - 1)) {
                clipRect.maxX = gRenderer.bufferWidth - 1;
            }
            if(tileY == (tileCountY - 1)) {
                clipRect.maxY = gRenderer.bufferHeight - 1;
            }
            param->clipRect = clipRect; 
            PlatformAddEntry(DoTileRenderWork, param);
        }
    }
    PlatformCompleteAllWork();
    gRenderer.workCount = 0;
#else
    rectangle2i clipRect = {0, 0, gRenderer.bufferWidth - 1, gRenderer.bufferHeight - 1};
    for(i32 i = 0; i < gRenderer.workCount; ++i) {
        RenderWork *work = gRenderer.workArray + i;
        RenderVertexArrayFast(work->vertices, work->indices,
                              work->indicesCount, work->bitmap, work->lightDir, work->world, clipRect);
    }
    gRenderer.workCount = 0;
#endif
}


void RendererSystemInitialize() {
    i32 bufferPitch = Align16(gWindow.width*4);
    i32 rendererWidth = bufferPitch/4;

    HDC hdc = GetDC(gWindow.hwnd);
    BITMAPINFO bufferInfo = {};
    bufferInfo.bmiHeader.biSize = sizeof(bufferInfo.bmiHeader);
    bufferInfo.bmiHeader.biWidth = rendererWidth;
    bufferInfo.bmiHeader.biHeight = gWindow.height;
    bufferInfo.bmiHeader.biPlanes = 1;
    bufferInfo.bmiHeader.biBitCount = 32;
    bufferInfo.bmiHeader.biCompression = BI_RGB;
    gRenderer.handle = CreateDIBSection(hdc, &bufferInfo, DIB_RGB_COLORS, (void **)&gRenderer.colorBuffer, 0, 0);
    gRenderer.hdc = hdc;
    gRenderer.depthBuffer = (f32 *)malloc(gWindow.width * gWindow.height * sizeof(f32));
    gRenderer.bufferWidth = gWindow.width;
    gRenderer.bufferHeight = gWindow.height;
    gRenderer.view = Mat4Identity();
    gRenderer.proj = Mat4Identity();
    gRenderer.workArray = (RenderWork *)malloc(sizeof(RenderWork) * 65536);
    gRenderer.workCount = 0;
}

void RendererSystemShutdown() {
    free(gRenderer.workArray);
    DeleteObject(gRenderer.handle);
    free(gRenderer.depthBuffer);
}

void RendererClearBuffers(u32 color, f32 depth) {
#if 1
    // TODO: test the cycles on this function
    __m128i pixelColor = _mm_set1_epi32(color);
    __m128 depthValue = _mm_set1_ps(depth);
    for(i32 y = 0; y < gRenderer.bufferHeight; ++y) {
        for(i32 x = 0; x < gRenderer.bufferWidth; x += 4) {
            u32 *pixelPt = gRenderer.colorBuffer + ((y * gRenderer.bufferWidth) + x);
            f32 *depthPt = gRenderer.depthBuffer + ((y * gRenderer.bufferWidth) + x);
            _mm_storeu_si128((__m128i *)pixelPt, pixelColor);
            _mm_storeu_ps(depthPt, depthValue);
        }
    }
#else
    for(i32 i = 0; i < gRenderer.bufferWidth*gRenderer.bufferHeight; ++i) {
        gRenderer.colorBuffer[i] = color;
        gRenderer.depthBuffer[i] = depth;
    }
#endif
}

void RendererPushWorkToQueue(Vertex *vertices, u32 *indices,
                             i32 indicesCount, Texture *bitmap, vec3 lightDir, mat4 world) {
    RenderWork *work = gRenderer.workArray + gRenderer.workCount++;
    work->vertices = vertices;
    work->verticesCount = 0;
    work->indices = indices;
    work->indicesCount = indicesCount;
    work->bitmap = bitmap;
    work->lightDir = lightDir;
    work->world = world;
}

void RendererPresent() {
    FlushWorkQueue();
    HDC colorBufferDC = CreateCompatibleDC(gRenderer.hdc);
    SelectObject(colorBufferDC, gRenderer.handle);
    BitBlt(gRenderer.hdc, 0, 0, gRenderer.bufferWidth, gRenderer.bufferHeight, colorBufferDC, 0, 0, SRCCOPY);
    DeleteDC(colorBufferDC);
}

void RendererSetProj(mat4 proj) {
    gRenderer.proj = proj;
} 

void RendererSetView(mat4 view) {
    gRenderer.view = view;
}

void DEBUG_RendererDrawWireframeBuffer(Vertex *vertices, i32 verticesCount, u32 color, mat4 world) {
    for(i32 i = 0; i < verticesCount; i += 3) {

        Vertex *aVertex = vertices + (i + 0);
        Vertex *bVertex = vertices + (i + 1);
        Vertex *cVertex = vertices + (i + 2);
        vec3 aTmp = aVertex->position;
        vec3 bTmp = bVertex->position;
        vec3 cTmp = cVertex->position;
        vec2 aUv = aVertex->uv;
        vec2 bUv = bVertex->uv;
        vec2 cUv = cVertex->uv;
        vec3 aNormal = aVertex->normal;
        vec3 bNormal = bVertex->normal;
        vec3 cNormal = cVertex->normal;

        vec4 a = {aTmp.x, aTmp.y, aTmp.z, 1.0f};
        vec4 b = {bTmp.x, bTmp.y, bTmp.z, 1.0f};
        vec4 c = {cTmp.x, cTmp.y, cTmp.z, 1.0f};

        // multiply by the world matrix...
        a = world * a;
        b = world * b;
        c = world * c;
        
        // normals in world space
        aNormal = Vec4ToVec3(world * Vec3ToVec4(aNormal, 0.0f));
        bNormal = Vec4ToVec3(world * Vec3ToVec4(bNormal, 0.0f));
        cNormal = Vec4ToVec3(world * Vec3ToVec4(cNormal, 0.0f));

        vec3 aFragPos = Vec4ToVec3(world * Vec3ToVec4(aTmp, 1.0f));
        vec3 bFragPos = Vec4ToVec3(world * Vec3ToVec4(bTmp, 1.0f));
        vec3 cFragPos = Vec4ToVec3(world * Vec3ToVec4(cTmp, 1.0f));

        // transform the vertices relative to the camera
        mat4 view = gRenderer.view;
        a = view * a;
        b = view * b;
        c = view * c;
        
        mat4 proj = gRenderer.proj;
        a = proj * a;
        b = proj * b;
        c = proj * c;

        i32 verticesACount = 3;
        vec4 verticesToClipA[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {a, b, c};
        vec2 uvsToClipA[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {aUv, bUv, cUv};
        vec3 normalsToClipA[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {aNormal, bNormal, cNormal};
        vec3 fragPosToClipA[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {aFragPos, bFragPos, cFragPos};

        i32 verticesBCount = 0;
        vec4 verticesToClipB[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {};
        vec2 uvsToClipB[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {};
        vec3 normalsToClipB[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {};
        vec3 fragPosToClipB[MAX_VERTICES_PER_CLIPPED_TRIANGLE] = {};
        HomogenousClipping(verticesToClipA, uvsToClipA, normalsToClipA, fragPosToClipA, verticesACount,
                           verticesToClipB, uvsToClipB, normalsToClipB, fragPosToClipB, &verticesBCount,
                           0, -1.0f);
        HomogenousClipping(verticesToClipB, uvsToClipB, normalsToClipB, fragPosToClipB, verticesBCount,
                           verticesToClipA, uvsToClipA, normalsToClipA, fragPosToClipA, &verticesACount,
                           0, 1.0f);
        HomogenousClipping(verticesToClipA, uvsToClipA, normalsToClipA, fragPosToClipA, verticesACount,
                           verticesToClipB, uvsToClipB, normalsToClipB, fragPosToClipB, &verticesBCount,
                           1, -1.0f);
        HomogenousClipping(verticesToClipB, uvsToClipB, normalsToClipB, fragPosToClipB, verticesBCount,
                           verticesToClipA, uvsToClipA, normalsToClipA, fragPosToClipA, &verticesACount,
                           1, 1.0f);
        HomogenousClipping(verticesToClipA, uvsToClipA, normalsToClipA, fragPosToClipA, verticesACount,
                           verticesToClipB, uvsToClipB, normalsToClipB, fragPosToClipB, &verticesBCount,
                           2, -1.0f);
        HomogenousClipping(verticesToClipB, uvsToClipB, normalsToClipB, fragPosToClipB, verticesBCount,
                           verticesToClipA, uvsToClipA, normalsToClipA, fragPosToClipA, &verticesACount,
                           2, 1.0f);
        for(i32 j = 0; j < verticesACount - 2; ++j) {
            vec4 newA = verticesToClipA[0];
            vec4 newB = verticesToClipA[1 + j];
            vec4 newC = verticesToClipA[2 + j]; 
            i32 halfBufferWidth = gRenderer.bufferWidth/2;
            i32 halfBufferHeight = gRenderer.bufferHeight/2;
            Point aPoint = {((newA.x / newA.w) * halfBufferWidth) + halfBufferWidth, ((newA.y / newA.w) * halfBufferHeight) + halfBufferHeight, newA.w};
            Point bPoint = {((newB.x / newB.w) * halfBufferWidth) + halfBufferWidth, ((newB.y / newB.w) * halfBufferHeight) + halfBufferHeight, newB.w};
            Point cPoint = {((newC.x / newC.w) * halfBufferWidth) + halfBufferWidth, ((newC.y / newC.w) * halfBufferHeight) + halfBufferHeight, newC.w};
            DrawLineTriangle(aPoint, bPoint, cPoint, color);
        }
    }
}


