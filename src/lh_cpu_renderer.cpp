#include "lh_cpu_renderer.h"
#include "lh_renderer.h"
#include "lh_platform.h"
#include <d3dcompiler.h>
#include <immintrin.h>
#include <xmmintrin.h>

extern Window gWindow;
extern Renderer gRenderer;

#define Align16(value) ((value + 15) & ~15)
#define MAX_VERTICES_PER_CLIPPED_TRIANGLE 16

// Vertex Shader
global_variable char *vertexShaderSource =
"struct VS_Input\n"
"{\n"
"   float4 pos : POSITION;\n"
"   float2 tex0 : TEXCOORD0;\n"
"};\n"
"struct PS_Input\n"
"{\n"
"   float4 pos : SV_POSITION;\n"
"   float2 tex0 : TEXCOORD0;\n"
"};\n"
"PS_Input VS_Main( VS_Input vertex )\n"
"{\n"
"   PS_Input vsOut = ( PS_Input )0;\n"
"   vsOut.pos = vertex.pos;\n"
"   vsOut.tex0 = vertex.tex0;\n"
"   return vsOut;\n"
"}\0";

// Pixel Shader
global_variable char *pixelShaderSource  =
"Texture2D colorMap : register( t0 );\n"
"SamplerState colorSampler : register( s0 );\n"
"struct PS_Input\n"
"{\n"
"   float4 pos : SV_POSITION;\n"
"   float2 tex0 : TEXCOORD0;\n"
"};\n"
"float4 PS_Main( PS_Input frag ) : SV_TARGET\n"
"{\n"
"   float4 color = colorMap.Sample(colorSampler, frag.tex0.xy);\n"
"   return float4(color.rgb, 1);\n"
"}\0";

internal
i32 StringLength(char * String)
{
    i32 Count = 0;
    while(*String++)
    {
        ++Count;
    }
    return Count;
}

struct ThreadParam {
    rectangle2i clipRect;
};

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
    CPURenderer *cpuRenderer = &gRenderer.cpuRenderer;
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
        if(x >= 0 && x < cpuRenderer->bufferWidth && y >= 0 && y < cpuRenderer->bufferHeight) {
            if(interpolatedReciprocalZ >= cpuRenderer->depthBuffer[(i32)y * cpuRenderer->bufferWidth + (i32)x]) {
                cpuRenderer->depthBuffer[(i32)y * cpuRenderer->bufferWidth + (i32)x] = interpolatedReciprocalZ;
                cpuRenderer->colorBuffer[(i32)y * cpuRenderer->bufferWidth + (i32)x] = color;
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
                        vec3 aFragPos, vec3 bFragPos, vec3 cFragPos, Texture *bitmap, vec3 *lights, i32 lightsCount, vec3 viewPos,
                        rectangle2i clipRect, bool writeDepthBuffer, f32 repeatU, f32 repeatV) {
    CPURenderer *cpuRenderer = &gRenderer.cpuRenderer;
    ASSERT(((uintptr_t)cpuRenderer->colorBuffer & 15) == 0);
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

        __m128 scaleU = _mm_set1_ps(repeatU);
        __m128 scaleV = _mm_set1_ps(repeatV);

        __m128 bitmapWidth = _mm_set1_ps((f32)bitmap->width - 1);
        __m128 bitmapHeight = _mm_set1_ps((f32)bitmap->height - 1);
        __m128 zero = _mm_set1_ps(0.0f);
        __m128 one = _mm_set1_ps(1.0f);
        __m128 two = _mm_set1_ps(2.0f);
        __m128 minusOne = _mm_set1_ps(-1.0f);
        __m128i u255 = _mm_set1_epi32(0xFF);
        __m128 f255 = _mm_set1_ps(255.0f);

        __m128 v0x = _mm_sub_ps(bPointX, aPointX);
        __m128 v0y = _mm_sub_ps(bPointY, aPointY);
        __m128 v1x = _mm_sub_ps(cPointX, aPointX);
        __m128 v1y = _mm_sub_ps(cPointY, aPointY);
        __m128 d00 = _mm_add_ps(_mm_mul_ps(v0x, v0x), _mm_mul_ps(v0y, v0y));
        __m128 d10 = _mm_add_ps(_mm_mul_ps(v1x, v0x), _mm_mul_ps(v1y, v0y));
        __m128 d11 = _mm_add_ps(_mm_mul_ps(v1x, v1x), _mm_mul_ps(v1y, v1y));
        __m128 denom = _mm_sub_ps(_mm_mul_ps(d00, d11), _mm_mul_ps(d10, d10));


        // vec3 viewPos = {0, -3, -8};
        __m128 viewPosX = _mm_set1_ps(viewPos.x);
        __m128 viewPosY = _mm_set1_ps(viewPos.y);
        __m128 viewPosZ = _mm_set1_ps(viewPos.z);

        //vec3 lightColor = {1, 1, 1};
        __m128 lightColorX = _mm_set1_ps(0.8f);
        __m128 lightColorY = _mm_set1_ps(0.7f);
        __m128 lightColorZ = _mm_set1_ps(0.2f);

        //__m128 diffuseColorX = _mm_set1_ps(1);
        //__m128 diffuseColorY = _mm_set1_ps(1);
        //__m128 diffuseColorZ = _mm_set1_ps(1);
        
        //vec3 lightColor = {1, 1, 1};
        //__m128 specularColorX = _mm_set1_ps(1);
        //__m128 specularColorY = _mm_set1_ps(1);
        //__m128 specularColorZ = _mm_set1_ps(1);
        
        __m128 ambientStrength  = _mm_set1_ps(0.2f);
        //__m128 diffuseStrength = _mm_set1_ps(0.6f);
        __m128 specularStrength = _mm_set1_ps(1.0f);

        __m128 specComponent = _mm_set1_ps(32.0f);

        __m128 constant = _mm_set1_ps(1.0f);
        __m128 linear = _mm_set1_ps(0.35f);
        __m128 quadratic = _mm_set1_ps(0.44f);

        i32 minX = fillRect.minX;
        i32 minY = fillRect.minY;
        i32 maxX = fillRect.maxX;
        i32 maxY = fillRect.maxY;

        for(i32 y = minY; y <= maxY; ++y) {
            __m128 pixelsToTestY = _mm_set1_ps(y);
            __m128i clipMask = startClipMask;
            for(i32 x = minX; x <= maxX; x += 4) {

                // get the old data for the maskout
                u32 *pixelPt = cpuRenderer->colorBuffer + ((y * cpuRenderer->bufferWidth) + x);
                __m128i originalDest = _mm_load_si128((__m128i *)pixelPt);
                
                f32 *depthPt = cpuRenderer->depthBuffer + ((y * cpuRenderer->bufferWidth) + x);
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

                        // TODO: if you want to repate the texture instead of streching 
                        interpolatedU = _mm_mul_ps(interpolatedU, scaleU);
                        interpolatedV = _mm_mul_ps(interpolatedV, scaleV);
                        for(i32 i = 0; i < 4; ++i) {
                            M(interpolatedU, i) = fmodf(M(interpolatedU, i), 1.0f);
                            M(interpolatedV, i) = fmodf(M(interpolatedV, i), 1.0f);
                        }


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
                        __m128 red   = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(color, 16), u255));
                        __m128 green = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(color, 8), u255));
                        __m128 blue  = _mm_cvtepi32_ps(_mm_and_si128(color, u255));
                        
                        vec3 lightPos = {-10, 6, 40}; 
                        __m128 lightPosX = _mm_set1_ps(lightPos.x);
                        __m128 lightPosY = _mm_set1_ps(lightPos.y);
                        __m128 lightPosZ = _mm_set1_ps(lightPos.z);
                        __m128 lightDirX = _mm_sub_ps(interpolatedFragPosX, lightPosX);
                        __m128 lightDirY = _mm_sub_ps(interpolatedFragPosY, lightPosY);
                        __m128 lightDirZ = _mm_sub_ps(interpolatedFragPosZ, lightPosZ);
                        squaredLength = _mm_add_ps(
                                             _mm_add_ps(_mm_mul_ps(lightDirX, lightDirX),
                                                        _mm_mul_ps(lightDirY, lightDirY)),
                                                        _mm_mul_ps(lightDirZ, lightDirZ));
                        length = _mm_sqrt_ps(squaredLength);
                        lightDirX = _mm_div_ps(lightDirX, length);
                        lightDirY = _mm_div_ps(lightDirY, length);
                        lightDirZ = _mm_div_ps(lightDirZ, length);

                        __m128 ambientX = _mm_mul_ps(lightColorX, ambientStrength);
                        __m128 ambientY = _mm_mul_ps(lightColorY, ambientStrength);
                        __m128 ambientZ = _mm_mul_ps(lightColorZ, ambientStrength);

                        __m128 diff =  _mm_min_ps(_mm_max_ps(
                                        _mm_add_ps(
                                             _mm_add_ps(_mm_mul_ps(normalizeInterpolatedNormalX, lightDirX),
                                                        _mm_mul_ps(normalizeInterpolatedNormalY, lightDirY)),
                                                        _mm_mul_ps(normalizeInterpolatedNormalZ, lightDirZ)),
                                       zero), one);

                        __m128 diffuseX = _mm_mul_ps(lightColorX, diff);
                        __m128 diffuseY = _mm_mul_ps(lightColorY, diff);
                        __m128 diffuseZ = _mm_mul_ps(lightColorZ, diff);

                        __m128 viewDirX = _mm_sub_ps(interpolatedFragPosX, viewPosX);
                        __m128 viewDirY = _mm_sub_ps(interpolatedFragPosY, viewPosY);
                        __m128 viewDirZ = _mm_sub_ps(interpolatedFragPosZ, viewPosZ);

                        squaredLength = _mm_add_ps(
                                        _mm_add_ps(_mm_mul_ps(viewDirX, viewDirX),
                                                   _mm_mul_ps(viewDirY, viewDirY)),
                                                   _mm_mul_ps(viewDirZ, viewDirZ));
                        length = _mm_sqrt_ps(squaredLength);
                        viewDirX = _mm_div_ps(viewDirX, length);
                        viewDirY = _mm_div_ps(viewDirY, length);
                        viewDirZ = _mm_div_ps(viewDirZ, length);

                        __m128 negativeLightDirX = _mm_mul_ps(lightDirX, minusOne);
                        __m128 negativeLightDirY = _mm_mul_ps(lightDirY, minusOne);
                        __m128 negativeLightDirZ = _mm_mul_ps(lightDirZ, minusOne);

                        squaredLength = _mm_add_ps(
                                        _mm_add_ps(_mm_mul_ps(normalizeInterpolatedNormalX, normalizeInterpolatedNormalX),
                                                   _mm_mul_ps(normalizeInterpolatedNormalY, normalizeInterpolatedNormalY)),
                                                   _mm_mul_ps(normalizeInterpolatedNormalZ, normalizeInterpolatedNormalZ));
                        length = _mm_sqrt_ps(squaredLength);
                            
                        __m128 scale = _mm_div_ps(
                                                  _mm_add_ps(
                                                  _mm_add_ps(_mm_mul_ps(negativeLightDirX, normalizeInterpolatedNormalX),
                                                             _mm_mul_ps(negativeLightDirY, normalizeInterpolatedNormalY)),
                                                             _mm_mul_ps(negativeLightDirZ, normalizeInterpolatedNormalZ)), length);
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

                        __m128 dotProduct = _mm_add_ps(
                                            _mm_add_ps(_mm_mul_ps(viewDirX, reflectDirX),
                                                       _mm_mul_ps(viewDirY, reflectDirY)),
                                                       _mm_mul_ps(viewDirZ, reflectDirZ));
                        __m128 spec = _mm_pow_ps(_mm_max_ps(dotProduct, zero), specComponent);
                        
                        __m128 specularX = _mm_mul_ps(_mm_mul_ps(lightColorX, spec), specularStrength);
                        __m128 specularY = _mm_mul_ps(_mm_mul_ps(lightColorY, spec), specularStrength);
                        __m128 specularZ = _mm_mul_ps(_mm_mul_ps(lightColorZ, spec), specularStrength);

                        __m128 resultX = _mm_mul_ps(_mm_add_ps(_mm_add_ps(ambientX, diffuseX), specularX), red);
                        __m128 resultY = _mm_mul_ps(_mm_add_ps(_mm_add_ps(ambientY, diffuseY), specularY), green);
                        __m128 resultZ = _mm_mul_ps(_mm_add_ps(_mm_add_ps(ambientZ, diffuseZ), specularZ), blue);

                        // clamp to 0-255 range
                        resultX = _mm_min_ps(_mm_max_ps(resultX, zero), f255);
                        resultY = _mm_min_ps(_mm_max_ps(resultY, zero), f255);
                        resultZ = _mm_min_ps(_mm_max_ps(resultZ, zero), f255);

                        __m128i r = _mm_cvtps_epi32(resultX);
                        __m128i g = _mm_cvtps_epi32(resultY);
                        __m128i b = _mm_cvtps_epi32(resultZ);
                        __m128i a = _mm_and_si128(_mm_srli_epi32(color, 24), u255);

                        __m128i sr = _mm_slli_epi32(r, 16);
                        __m128i sg = _mm_slli_epi32(g, 8);
                        __m128i sb = b;
                        __m128i sa = _mm_slli_epi32(a, 24);
                        if(lightsCount) { 
                            color = _mm_or_si128(_mm_or_si128(sr, sg), _mm_or_si128(sb, sa));
                        }
#else
                        __m128 finalColorX = _mm_set1_ps(0);
                        __m128 finalColorY = _mm_set1_ps(0);
                        __m128 finalColorZ = _mm_set1_ps(0);
                        for(i32 i = 0; i < lightsCount; ++i) {
                            vec3 lightPos = lights[i];
                            //vec3 lightPos = {3, -3.5f, -4};
                            __m128 lightPosX = _mm_set1_ps(lightPos.x);
                            __m128 lightPosY = _mm_set1_ps(lightPos.y);
                            __m128 lightPosZ = _mm_set1_ps(lightPos.z);
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
                            __m128 red   = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(color, 16), u255));
                            __m128 green = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(color, 8), u255));
                            __m128 blue  = _mm_cvtepi32_ps(_mm_and_si128(color, u255));

                            __m128 lightDirX = _mm_sub_ps(interpolatedFragPosX, lightPosX);
                            __m128 lightDirY = _mm_sub_ps(interpolatedFragPosY, lightPosY);
                            __m128 lightDirZ = _mm_sub_ps(interpolatedFragPosZ, lightPosZ);

                            __m128 distanceSq = _mm_add_ps(
                                                _mm_add_ps(_mm_mul_ps(lightDirX, lightDirX),
                                                           _mm_mul_ps(lightDirY, lightDirY)),
                                                           _mm_mul_ps(lightDirZ, lightDirZ));
                            __m128 distance = _mm_sqrt_ps(distanceSq);

                            lightDirX = _mm_div_ps(lightDirX, distance);
                            lightDirY = _mm_div_ps(lightDirY, distance);
                            lightDirZ = _mm_div_ps(lightDirZ, distance);

                            __m128 attenuation = _mm_div_ps(one,
                                                        _mm_add_ps(
                                                        _mm_add_ps(constant, 
                                                               _mm_mul_ps(linear, distance)),
                                                               _mm_mul_ps(quadratic, distanceSq)));


                            __m128 negativeLightDirX = _mm_mul_ps(lightDirX, minusOne);
                            __m128 negativeLightDirY = _mm_mul_ps(lightDirY, minusOne);
                            __m128 negativeLightDirZ = _mm_mul_ps(lightDirZ, minusOne);

                            __m128 viewDirX = _mm_sub_ps(interpolatedFragPosX, viewPosX);
                            __m128 viewDirY = _mm_sub_ps(interpolatedFragPosY, viewPosY);
                            __m128 viewDirZ = _mm_sub_ps(interpolatedFragPosZ, viewPosZ);

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

                            __m128 diffuseX = _mm_mul_ps(lightColorX, diff);
                            __m128 diffuseY = _mm_mul_ps(lightColorY, diff);
                            __m128 diffuseZ = _mm_mul_ps(lightColorZ, diff);

                            __m128 dotProduct = _mm_add_ps(
                                                _mm_add_ps(_mm_mul_ps(viewDirX, reflectDirX),
                                                           _mm_mul_ps(viewDirY, reflectDirY)),
                                                           _mm_mul_ps(viewDirZ, reflectDirZ));


#if 1
                            __m128 spec = _mm_pow_ps(_mm_max_ps(dotProduct, zero), specComponent);
#else
                            dotProduct = _mm_max_ps(dotProduct, zero);
                            __m128 spec = _mm_mul_ps(dotProduct, dotProduct);
#endif

                            __m128 specularX = _mm_mul_ps(_mm_mul_ps(lightColorX, spec), specularStrength);
                            __m128 specularY = _mm_mul_ps(_mm_mul_ps(lightColorY, spec), specularStrength);
                            __m128 specularZ = _mm_mul_ps(_mm_mul_ps(lightColorZ, spec), specularStrength);

                            ambientX = _mm_mul_ps(ambientX, attenuation);
                            ambientY = _mm_mul_ps(ambientY, attenuation);
                            ambientZ = _mm_mul_ps(ambientZ, attenuation);
                            diffuseX = _mm_mul_ps(diffuseX, attenuation);
                            diffuseY = _mm_mul_ps(diffuseY, attenuation);
                            diffuseZ = _mm_mul_ps(diffuseZ, attenuation);
                            specularX = _mm_mul_ps(specularX, attenuation);
                            specularY = _mm_mul_ps(specularY, attenuation);
                            specularZ = _mm_mul_ps(specularZ, attenuation);

                            __m128 resultX = _mm_mul_ps(_mm_add_ps(_mm_add_ps(ambientX, diffuseX), specularX), red);
                            __m128 resultY = _mm_mul_ps(_mm_add_ps(_mm_add_ps(ambientY, diffuseY), specularY), green);
                            __m128 resultZ = _mm_mul_ps(_mm_add_ps(_mm_add_ps(ambientZ, diffuseZ), specularZ), blue);

                            // clamp to 0-255 range
                            resultX = _mm_min_ps(_mm_max_ps(resultX, zero), f255);
                            resultY = _mm_min_ps(_mm_max_ps(resultY, zero), f255);
                            resultZ = _mm_min_ps(_mm_max_ps(resultZ, zero), f255);

                            finalColorX = _mm_add_ps(finalColorX, resultX);
                            finalColorY = _mm_add_ps(finalColorY, resultY);
                            finalColorZ = _mm_add_ps(finalColorZ, resultZ);

                        }
                        finalColorX = _mm_min_ps(_mm_max_ps(finalColorX, zero), f255);
                        finalColorY = _mm_min_ps(_mm_max_ps(finalColorY, zero), f255);
                        finalColorZ = _mm_min_ps(_mm_max_ps(finalColorZ, zero), f255);

                        __m128i r = _mm_cvtps_epi32(finalColorX);
                        __m128i g = _mm_cvtps_epi32(finalColorY);
                        __m128i b = _mm_cvtps_epi32(finalColorZ);
                        __m128i a = _mm_and_si128(_mm_srli_epi32(color, 24), u255);

                        __m128i sr = _mm_slli_epi32(r, 16);
                        __m128i sg = _mm_slli_epi32(g, 8);
                        __m128i sb = b;
                        __m128i sa = _mm_slli_epi32(a, 24);
                        if(lightsCount) { 
                            color = _mm_or_si128(_mm_or_si128(sr, sg), _mm_or_si128(sb, sa));
                        }

#endif
                        // alpha blending
                        {
                        __m128 a = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(color, 24), u255));
                        __m128 invA =_mm_div_ps(a, f255); 

                        __m128 srcR = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(color, 16), u255));
                        __m128 srcG = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(color, 8), u255));
                        __m128 srcB = _mm_cvtepi32_ps(_mm_and_si128(color, u255));

                        __m128 dstR = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(originalDest, 16), u255));
                        __m128 dstG = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(originalDest, 8), u255));
                        __m128 dstB = _mm_cvtepi32_ps(_mm_and_si128(originalDest, u255));

                        __m128 r = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one, invA), dstR), _mm_mul_ps(invA, srcR));
                        __m128 g = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one, invA), dstG), _mm_mul_ps(invA, srcG));
                        __m128 b = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one, invA), dstB), _mm_mul_ps(invA, srcB));
                        
                        color = _mm_or_si128(
                                _mm_or_si128(_mm_slli_epi32(_mm_cvtps_epi32(a), 24), _mm_slli_epi32(_mm_cvtps_epi32(r), 16)),
                                _mm_or_si128(_mm_slli_epi32(_mm_cvtps_epi32(g),  8), _mm_cvtps_epi32(b)));
                        }

                        __m128i colorMaskedOut = _mm_or_si128(_mm_and_si128(writeMaski, color), _mm_andnot_si128(writeMaski, originalDest));
                        __m128 depthMaskOut = _mm_or_ps(_mm_and_ps(writeMask, interReciZ), _mm_andnot_ps(writeMask, depth));
                        _mm_store_si128((__m128i *)pixelPt, colorMaskedOut);
                        if(writeDepthBuffer) {
                            _mm_store_ps(depthPt, depthMaskOut);
                        }
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
                           i32 indicesCount, Texture *bitmap, vec3 *lights, i32 lightsCount, vec3 viewPos,
                           mat4 world, rectangle2i clipRect, bool writeDepthBuffer, f32 repeatU, f32 repeatV) {    
    CPURenderer *cpuRenderer = &gRenderer.cpuRenderer;
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
        mat4 view = cpuRenderer->view;
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

        mat4 proj = cpuRenderer->proj;
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
            i32 halfBufferWidth =  cpuRenderer->bufferWidth/2;
            i32 halfBufferHeight = cpuRenderer->bufferHeight/2;
            Point aPoint = {((newA.x * aInvW) * halfBufferWidth) + halfBufferWidth, ((newA.y * aInvW) * halfBufferHeight) + halfBufferHeight, aInvW};
            Point bPoint = {((newB.x * bInvW) * halfBufferWidth) + halfBufferWidth, ((newB.y * bInvW) * halfBufferHeight) + halfBufferHeight, bInvW};
            Point cPoint = {((newC.x * cInvW) * halfBufferWidth) + halfBufferWidth, ((newC.y * cInvW) * halfBufferHeight) + halfBufferHeight, cInvW};
            TriangleRasterizer(aPoint, bPoint, cPoint,
                               newUvA, newUvB, newUvC,
                               newNormalA, newNormalB, newNormalC,
                               newFragPosA, newFragPosB, newFragPosC,
                               bitmap,
                               lights, lightsCount, viewPos,
                               clipRect, writeDepthBuffer,
                               repeatU, repeatV);
        }
    }
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
        mat4 view = gRenderer.cpuRenderer.view;
        a = view * a;
        b = view * b;
        c = view * c;
        
        mat4 proj = gRenderer.cpuRenderer.proj;
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
            i32 halfBufferWidth = gRenderer.cpuRenderer.bufferWidth/2;
            i32 halfBufferHeight = gRenderer.cpuRenderer.bufferHeight/2;
            Point aPoint = {((newA.x / newA.w) * halfBufferWidth) + halfBufferWidth, ((newA.y / newA.w) * halfBufferHeight) + halfBufferHeight, newA.w};
            Point bPoint = {((newB.x / newB.w) * halfBufferWidth) + halfBufferWidth, ((newB.y / newB.w) * halfBufferHeight) + halfBufferHeight, newB.w};
            Point cPoint = {((newC.x / newC.w) * halfBufferWidth) + halfBufferWidth, ((newC.y / newC.w) * halfBufferHeight) + halfBufferHeight, newC.w};
            DrawLineTriangle(aPoint, bPoint, cPoint, color);
        }
    }
}


internal
void DoTileRenderWork(void *data) {
    CPURenderer *cpuRenderer = &gRenderer.cpuRenderer;
    ThreadParam *param = (ThreadParam *)data;
    for(i32 i = 0; i < cpuRenderer->workCount; ++i) {
        RenderWork *work = cpuRenderer->workArray + i;
        RenderVertexArrayFast(work->vertices, work->indices,
                              work->indicesCount, work->bitmap,
                              work->lights, work->lightsCount,
                              work->viewPos, work->world,
                              param->clipRect, work->writeDepthBuffer,
                              work->repeatU, work->repeatV);
    }
}

internal
void CPURendererPushWorkToQueue(Vertex *vertices, u32 *indices,
                                i32 indicesCount, Texture *bitmap, vec3 *lights, i32 lightsCount,
                                vec3 viewPos, mat4 world, bool writeDepthBuffer, f32 repeatU, f32 repeatV) {
    CPURenderer *cpuRenderer = &gRenderer.cpuRenderer;
    RenderWork *work = cpuRenderer->workArray + cpuRenderer->workCount++;
    work->vertices = vertices;
    work->verticesCount = 0;
    work->indices = indices;
    work->indicesCount = indicesCount;
    work->bitmap = bitmap;
    work->lights = lights;
    work->lightsCount = lightsCount;
    work->viewPos = viewPos;
    work->world = world;
    work->writeDepthBuffer = writeDepthBuffer;
    work->repeatU = repeatU;
    work->repeatV = repeatV;               
}


void RendererFlushWorkQueue() {
    if(gRenderer.type == RENDERER_CPU) {
        CPURenderer *cpuRenderer = &gRenderer.cpuRenderer;
        const i32 tileCountX = 4;
        const i32 tileCountY = 4;
        ThreadParam paramArray[tileCountX*tileCountY];
        i32 tileWidth = cpuRenderer->bufferWidth / tileCountX;
        i32 tileHeight = cpuRenderer->bufferHeight / tileCountY;
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
                    clipRect.maxX = cpuRenderer->bufferWidth - 1;
                }
                if(tileY == (tileCountY - 1)) {
                    clipRect.maxY = cpuRenderer->bufferHeight - 1;
                }
                param->clipRect = clipRect; 
                PlatformAddEntry(DoTileRenderWork, param);
            }
        }
        PlatformCompleteAllWork();
        cpuRenderer->workCount = 0;
    }    
}


void CPURendererInitialize() {
    CPURenderer *cpuRenderer = &gRenderer.cpuRenderer;

    i32 bufferPitch = Align16(gWindow.width*4);
    i32 rendererWidth = bufferPitch/4;
    cpuRenderer->colorBuffer = (u32 *)malloc(rendererWidth * gWindow.height * sizeof(u32));
    cpuRenderer->depthBuffer = (f32 *)malloc(gWindow.width * gWindow.height * sizeof(f32));
    cpuRenderer->bufferWidth = gWindow.width;
    cpuRenderer->bufferHeight = gWindow.height;
    cpuRenderer->view = Mat4Identity();
    cpuRenderer->proj = Mat4Identity();
    cpuRenderer->workArray = (RenderWork *)malloc(sizeof(RenderWork) * 65536);
    cpuRenderer->workCount = 0;
    
    // - 5: Create Vertex, Pixel shader and Input Layout
    HRESULT result;
    ID3DBlob *vertexShaderCompiled = 0;
    ID3DBlob *errorVertexShader    = 0;
    result = D3DCompile((void *)vertexShaderSource,
                        (SIZE_T)StringLength(vertexShaderSource),
                        0, 0, 0, "VS_Main", "vs_4_0",
                        D3DCOMPILE_ENABLE_STRICTNESS, 0,
                        &vertexShaderCompiled, &errorVertexShader);
    if(errorVertexShader != 0)
    {
        errorVertexShader->Release();
    }

    ID3DBlob *pixelShaderCompiled = 0;
    ID3DBlob *errorPixelShader    = 0;
    result = D3DCompile((void *)pixelShaderSource,
                        (SIZE_T)StringLength(pixelShaderSource),
                        0, 0, 0, "PS_Main", "ps_4_0",
                        D3DCOMPILE_ENABLE_STRICTNESS, 0,
                        &pixelShaderCompiled, &errorPixelShader);
    if(errorPixelShader != 0)
    {
        errorPixelShader->Release();
    }

    // Create the Vertex Shader.
    result = gRenderer.device->CreateVertexShader(vertexShaderCompiled->GetBufferPointer(),
                                                  vertexShaderCompiled->GetBufferSize(), 0,
                                                  &cpuRenderer->vertexShader);
    // Create the Input layout.
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
         0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
        0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    u32 totalLayoutElements = ARRAY_LENGTH(inputLayoutDesc);
    result = gRenderer.device->CreateInputLayout(inputLayoutDesc,
                                                 totalLayoutElements,
                                                 vertexShaderCompiled->GetBufferPointer(),
                                                 vertexShaderCompiled->GetBufferSize(),
                                                 &cpuRenderer->inputLayout);
    // Create Pixel Shader.
    result = gRenderer.device->CreatePixelShader(pixelShaderCompiled->GetBufferPointer(),
                                                 pixelShaderCompiled->GetBufferSize(), 0,
                                                 &cpuRenderer->pixelShader); 
    vertexShaderCompiled->Release();
    pixelShaderCompiled->Release();

    // Create Vertex Buffer
    // TODO: FIX THIS ...
    f32 oneX = -(1.0f + (2.0f / (f32)WINDOW_WIDTH));
    f32 oneY = -(1.0f + (2.0f / (f32)WINDOW_HEIGHT));
    VertexD3D11 vertices[] = 
    {
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f,  oneY, 0.0f, 1.0f, 0.0f,
         oneX,  oneY, 0.0f, 0.0f, 0.0f,
         oneX,  oneY, 0.0f, 0.0f, 0.0f,
         oneX,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
    };

    // buffer description
    D3D11_BUFFER_DESC vertexDesc = {};
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = sizeof(VertexD3D11) * 6;
    // pass the buffer data (Vertices).
    D3D11_SUBRESOURCE_DATA resourceData = {};
    resourceData.pSysMem = vertices;
    // Create the VertexBuffer
    result = gRenderer.device->CreateBuffer(&vertexDesc, &resourceData, &cpuRenderer->vertexBuffer);

    i32 clientWidth = gWindow.width;
    i32 clientHeight = gWindow.height;

    D3D11_TEXTURE2D_DESC textureDesc = {}; 
    textureDesc.Width = cpuRenderer->bufferWidth;
    textureDesc.Height = cpuRenderer->bufferHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DYNAMIC;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    textureDesc.MiscFlags = 0;
    // Create out Texture 
    result = gRenderer.device->CreateTexture2D(&textureDesc, NULL, &cpuRenderer->backBuffer);
    if(SUCCEEDED(result))
    {
        OutputDebugString("SUCCEEDED Creating Texture\n");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc = {};
    shaderResourceDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceDesc.Texture2D.MipLevels = 1;
    result = gRenderer.device->CreateShaderResourceView(cpuRenderer->backBuffer, &shaderResourceDesc, &cpuRenderer->colorMap);
    if(SUCCEEDED(result))
    {
        OutputDebugString("SUCCEEDED Creating Shader resource view\n");
    }

    D3D11_SAMPLER_DESC colorMapDesc = {};
    colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //D3D11_FILTER_MIN_MAG_MIP_LINEAR | D3D11_FILTER_MIN_MAG_MIP_POINT
    colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
    result = gRenderer.device->CreateSamplerState(&colorMapDesc, &cpuRenderer->colorMapSampler);
    if(SUCCEEDED(result))
    {
        OutputDebugString("SUCCEEDED Creating sampler state\n");
    }

}

void CPURendererShutdown() {
    CPURenderer *cpuRenderer = &gRenderer.cpuRenderer;
    cpuRenderer->colorMapSampler->Release();
    cpuRenderer->colorMap->Release();
    cpuRenderer->backBuffer->Release();
    cpuRenderer->vertexBuffer->Release();
    cpuRenderer->inputLayout->Release();
    cpuRenderer->pixelShader->Release();
    cpuRenderer->vertexShader->Release();
    free(cpuRenderer->workArray);
    free(cpuRenderer->depthBuffer);
    free(cpuRenderer->colorBuffer);
}

void CPURendererDrawMesh(Mesh *mesh, mat4 world, Texture *texture, vec3 *lights, i32 lightsCount,
                         vec3 viewPos, bool writeDepthBuffer, f32 repeatU, f32 repeatV) {
    CPURendererPushWorkToQueue(mesh->vertices, mesh->indices, mesh->indicesCount, texture, lights, lightsCount,
                               viewPos, world, writeDepthBuffer, repeatU, repeatV);
}

void CPUDrawRectFast(i32 x, i32 y, i32 width, i32 height, Texture *bitmap) {
    CPURenderer *cpuRenderer = &gRenderer.cpuRenderer;
    i32 minX = x;
    i32 minY = y;
    i32 maxX = x + width;
    i32 maxY = y + height;
    
    i32 offsetX = 0;
    i32 offsetY = 0;
    if(minX < 0)
    {
        offsetX = -minX;
        minX = 0;
    }
    if(maxX > cpuRenderer->bufferWidth)
    {
        maxX = cpuRenderer->bufferWidth;
    }
    if(minY < 0)
    {
        offsetY = -minY;
        minY = 0;
    }
    if(maxY > cpuRenderer->bufferHeight)
    {
        maxY = cpuRenderer->bufferHeight;
    }

    f32 ratioU = (f32)bitmap->width / width;
    f32 ratioV = (f32)bitmap->height / height;
 
    u32 *srcBuffer = (u32 *)bitmap->data;

    __m128i u255 = _mm_set1_epi32(0xFF);
    __m128 f255 = _mm_set1_ps(255.0f);
    __m128 one = _mm_set1_ps(1.0f);
    
    i32 counterY = offsetY;
    for(i32 y = minY; y < maxY; ++y)
    {
        i32 counterX = offsetX;
        for(i32 x = minX; x < (maxX - 3); x += 4)
        {
            u32 *dst = cpuRenderer->colorBuffer + (y * cpuRenderer->bufferWidth + x);
            __m128i oldTexel = _mm_loadu_si128((__m128i *)dst);
            
            i32 texY = (i32)(counterY * ratioV); 
            i32 texX = (i32)((f32)(counterX + 0) * ratioU);
            
            __m128i texel;
            Mu(texel, 0) = *(srcBuffer + (texY * (i32)bitmap->width + texX));
            texX = (i32)((f32)(counterX + 1) * ratioU);
            Mu(texel, 1) = *(srcBuffer + (texY * (i32)bitmap->width + texX));
            texX = (i32)((f32)(counterX + 2) * ratioU);
            Mu(texel, 2) = *(srcBuffer + (texY * (i32)bitmap->width + texX));
            texX = (i32)((f32)(counterX + 3) * ratioU);
            Mu(texel, 3) = *(srcBuffer + (texY * (i32)bitmap->width + texX));

            if(_mm_movemask_epi8(texel))
            {
                __m128 a = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel, 24), u255));
                __m128 invA =_mm_div_ps(a, f255); 

                __m128 srcR = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel, 16), u255));
                __m128 srcG = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel, 8), u255));
                __m128 srcB = _mm_cvtepi32_ps(_mm_and_si128(texel, u255));

                __m128 dstR = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(oldTexel, 16), u255));
                __m128 dstG = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(oldTexel, 8), u255));
                __m128 dstB = _mm_cvtepi32_ps(_mm_and_si128(oldTexel, u255));

                __m128 r = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one, invA), dstR), _mm_mul_ps(invA, srcR));
                __m128 g = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one, invA), dstG), _mm_mul_ps(invA, srcG));
                __m128 b = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one, invA), dstB), _mm_mul_ps(invA, srcB));
                
                __m128i color = _mm_or_si128(
                                _mm_or_si128(_mm_slli_epi32(_mm_cvtps_epi32(a), 24), _mm_slli_epi32(_mm_cvtps_epi32(r), 16)),
                                _mm_or_si128(_mm_slli_epi32(_mm_cvtps_epi32(g),  8), _mm_cvtps_epi32(b)));

                _mm_storeu_si128((__m128i *)dst, color);
            }
            counterX += 4; 
        }
        ++counterY;
    }
}

void CPUDrawAnimatedRectFast(i32 x, i32 y, i32 width, i32 height, Texture *bitmap, i32 spriteW, i32 spriteH, i32 frame) {
    CPURenderer *cpuRenderer = &gRenderer.cpuRenderer;
    i32 minX = x;
    i32 minY = y;
    i32 maxX = x + width;
    i32 maxY = y + height;
    
    i32 offsetX = 0;
    i32 offsetY = 0;
    if(minX < 0)
    {
        offsetX = -minX;
        minX = 0;
    }
    if(maxX > cpuRenderer->bufferWidth)
    {
        maxX = cpuRenderer->bufferWidth;
    }
    if(minY < 0)
    {
        offsetY = -minY;
        minY = 0;
    }
    if(maxY > cpuRenderer->bufferHeight)
    {
        maxY = cpuRenderer->bufferHeight;
    }


    i32 offsetU = frame % (bitmap->width / spriteW) * spriteW;
    i32 offsetV = frame / (bitmap->width / spriteW) * spriteH;
    f32 ratioU = (f32)spriteW / width;
    f32 ratioV = (f32)spriteH / height;
 
    u32 *srcBuffer = (u32 *)bitmap->data;

    __m128i u255 = _mm_set1_epi32(0xFF);
    __m128 f255 = _mm_set1_ps(255.0f);
    __m128 one = _mm_set1_ps(1.0f);
    
    i32 counterY = offsetY;
    for(i32 y = minY; y < maxY; ++y)
    {
        i32 counterX = offsetX;
        for(i32 x = minX; x < (maxX - 3); x += 4)
        {
            u32 *dst = cpuRenderer->colorBuffer + (y * cpuRenderer->bufferWidth + x);
            __m128i oldTexel = _mm_loadu_si128((__m128i *)dst);
            
            i32 texY = (i32)(counterY * ratioV); 
            i32 texX = (i32)((f32)(counterX + 0) * ratioU);
            
            __m128i texel;
            Mu(texel, 0) = *(srcBuffer + ((texY + offsetV) * (i32)bitmap->width + (texX + offsetU)));
            texX = (i32)((f32)(counterX + 1) * ratioU);
            Mu(texel, 1) = *(srcBuffer + ((texY + offsetV) * (i32)bitmap->width + (texX + offsetU)));
            texX = (i32)((f32)(counterX + 2) * ratioU);
            Mu(texel, 2) = *(srcBuffer + ((texY + offsetV) * (i32)bitmap->width + (texX + offsetU)));
            texX = (i32)((f32)(counterX + 3) * ratioU);
            Mu(texel, 3) = *(srcBuffer + ((texY + offsetV) * (i32)bitmap->width + (texX + offsetU)));

            if(_mm_movemask_epi8(texel))
            {
                __m128 a = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel, 24), u255));
                __m128 invA =_mm_div_ps(a, f255); 

                __m128 srcR = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel, 16), u255));
                __m128 srcG = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(texel, 8), u255));
                __m128 srcB = _mm_cvtepi32_ps(_mm_and_si128(texel, u255));

                __m128 dstR = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(oldTexel, 16), u255));
                __m128 dstG = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(oldTexel, 8), u255));
                __m128 dstB = _mm_cvtepi32_ps(_mm_and_si128(oldTexel, u255));

                __m128 r = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one, invA), dstR), _mm_mul_ps(invA, srcR));
                __m128 g = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one, invA), dstG), _mm_mul_ps(invA, srcG));
                __m128 b = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one, invA), dstB), _mm_mul_ps(invA, srcB));
                
                __m128i color = _mm_or_si128(
                                _mm_or_si128(_mm_slli_epi32(_mm_cvtps_epi32(a), 24), _mm_slli_epi32(_mm_cvtps_epi32(r), 16)),
                                _mm_or_si128(_mm_slli_epi32(_mm_cvtps_epi32(g),  8), _mm_cvtps_epi32(b)));

                _mm_storeu_si128((__m128i *)dst, color);
            }
            counterX += 4; 
        }
        ++counterY;
    }
}

void CPUDrawRect(i32 xPos, i32 yPos, i32 width, i32 height, Texture *bitmap) {
    CPURenderer *cpuRenderer = &gRenderer.cpuRenderer;
    u32 *pixels = (u32 *)bitmap->data;
    for(i32 y = yPos; y < height + yPos; ++y) {
        for(i32 x = xPos; x < width + xPos; ++x) {
            f32 xRatio = (f32)(x - xPos) / (f32)width; 
            f32 yRatio = (f32)(y - yPos) / (f32)height;
            i32 xPixel = bitmap->width  * xRatio;
            i32 yPixel = bitmap->height * yRatio;
            
            u32 srcColor = pixels[yPixel * bitmap->width + xPixel]; 
            f32 srcR = (f32)((srcColor >> 16) & 0xFF);
            f32 srcG = (f32)((srcColor >>  8) & 0xFF);
            f32 srcB = (f32)((srcColor >>  0) & 0xFF);
            
            u32 dstColor = cpuRenderer->colorBuffer[y * cpuRenderer->bufferWidth + x]; 
            f32 dstR = (f32)((dstColor >> 16) & 0xFF);
            f32 dstG = (f32)((dstColor >>  8) & 0xFF);
            f32 dstB = (f32)((dstColor >>  0) & 0xFF);

            f32 t = 1.0f; //(f32)((srcColor >> 24) & 0xFF) / 255.0f;
            u32 r = (u32)((1.0f - t) * dstR + t * srcR);
            u32 g = (u32)((1.0f - t) * dstG + t * srcG);
            u32 b = (u32)((1.0f - t) * dstB + t * srcB);

            u32 color = r << 16 | g << 8 | b;

            cpuRenderer->colorBuffer[y * cpuRenderer->bufferWidth + x] = color;
        }
    }
}


