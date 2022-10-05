internal
void SwapPoint(Point *a, Point *b) {
    Point tmp = *a;
    *a = *b;
    *b = tmp;
}

internal
void SwapVec2(vec2 *a, vec2 *b) {
    vec2 tmp = *a;
    *a = *b;
    *b = tmp;
}

internal 
void SwapInt(i32 *a, i32 *b) {
    i32 tmp = *a;
    *a = *b;
    *b = tmp;
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

inline
u32 BitScanForward(u32 mask)
{
    unsigned long shift = 0;
    _BitScanForward(&shift, mask);
    return (u32)shift;
}

Bitmap LoadBitmap(char *path, Arena *arena) {
    ReadFileResult fileResult = ReadFile(path, arena);
    BitmapHeader *header = (BitmapHeader *)fileResult.data;
    Bitmap bitmap;
    bitmap.data = (void *)((u8 *)fileResult.data + header->bitmapOffset);
    bitmap.width = header->width;
    bitmap.height = header->height;
    u32 redShift = BitScanForward(header->redMask);
    u32 greenShift = BitScanForward(header->greenMask);
    u32 blueShift = BitScanForward(header->blueMask);
    u32 alphaShift = BitScanForward(header->alphaMask);
    u32 *colorData = (u32 *)bitmap.data;
    for(u32 i = 0; i < bitmap.width*bitmap.height; ++i)
    {
        u32 red = (colorData[i] & header->redMask) >> redShift;       
        u32 green = (colorData[i] & header->greenMask) >> greenShift;       
        u32 blue = (colorData[i] & header->blueMask) >> blueShift;       
        u32 alpha = (colorData[i] & header->alphaMask) >> alphaShift;       
        colorData[i] = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
    }
    return bitmap;
}

internal
void RenderBuffer(Renderer *renderer, vec3 *vertices, i32 verticesCount) {

    // render the vertices list from a camera point of view
    mat4 view = Mat4LookAt({0, 0, -8}, {0, 0, 0}, {0, 1, 0}); 
    mat4 proj = Mat4Perspective(60.0f, 800.0f/600.0f, 0.1f, 100.0f);
    //mat4 proj = Mat4Ortho(-400, 400, -300, 300, 0.1f, 100.0f);
    local_persist f32 angle = 0.0f;
    mat4 rotY = Mat4RotateY(RAD(angle));
    mat4 rotX = Mat4RotateX(RAD(angle));
    mat4 rotZ = Mat4RotateZ(RAD(angle));
    mat4 world = rotY * rotX * rotZ;
    angle += 1.0f;

    for(i32 i = 0; i < verticesCount; i += 3) {
        vec3 aTmp = vertices[i + 0];
        vec3 bTmp = vertices[i + 1];
        vec3 cTmp = vertices[i + 2];

        vec4 a = {aTmp.x, aTmp.y, aTmp.z, 1.0f};
        vec4 b = {bTmp.x, bTmp.y, bTmp.z, 1.0f};
        vec4 c = {cTmp.x, cTmp.y, cTmp.z, 1.0f};

        // multiply by the world matrix...
        a = world * a;
        b = world * b;
        c = world * c;

        // transform the vertices relative to the camera
        a = view * a;
        b = view * b;
        c = view * c;

        // backface culling
        vec3 vecA = Vec4ToVec3(a);
        vec3 ab = Vec4ToVec3(b) - vecA;
        vec3 ac = Vec4ToVec3(c) - vecA;
        vec3 normal = normalized(cross(ac, ab));
        vec3 origin = {0, 0, 0};
        vec3 cameraRay = origin - vecA;
        f32 normalDirection = dot(normal, cameraRay);
        if(normalDirection < 0.0f) {
            continue;
        }

        a = proj * a;
        b = proj * b;
        c = proj * c;
        
        Point aPoint = {((a.x / a.w) * 400) + 400, ((a.y / a.w) * 300) + 300, a.w};
        Point bPoint = {((b.x / b.w) * 400) + 400, ((b.y / b.w) * 300) + 300, b.w};
        Point cPoint = {((c.x / c.w) * 400) + 400, ((c.y / c.w) * 300) + 300, c.w};
        
        DrawFillTriangle(renderer, aPoint, bPoint, cPoint, 0x00FF0000);
        // render DEBUG wireframe 
        DrawLineTriangle(renderer, aPoint, bPoint, cPoint, 0x0000FF00);
    }
}

internal
void RenderBufferTexture(Renderer *renderer, vec3 *vertices, vec2 *uvs, i32 verticesCount, Bitmap bitmap) {
    // render the vertices list from a camera point of view
    mat4 view = Mat4LookAt({0, 0, -8}, {0, 0, 0}, {0, 1, 0}); 
    mat4 proj = Mat4Perspective(60.0f, 800.0f/600.0f, 0.1f, 100.0f);
    //mat4 proj = Mat4Ortho(-400, 400, -300, 300, 0.1f, 100.0f);
    local_persist f32 angle = 0.0f;
    mat4 rotY = Mat4RotateY(RAD(angle));
    mat4 rotX = Mat4RotateX(RAD(angle));
    mat4 rotZ = Mat4RotateZ(RAD(angle));
    mat4 world = rotY * rotX * rotZ;
    angle += 1.0f;
    
    for(i32 i = 0; i < verticesCount; i += 3) {
        vec3 aTmp = vertices[i + 0];
        vec3 bTmp = vertices[i + 1];
        vec3 cTmp = vertices[i + 2];

        vec2 aUv = uvs[i + 0];
        vec2 bUv = uvs[i + 1];
        vec2 cUv = uvs[i + 2];

        vec4 a = {aTmp.x, aTmp.y, aTmp.z, 1.0f};
        vec4 b = {bTmp.x, bTmp.y, bTmp.z, 1.0f};
        vec4 c = {cTmp.x, cTmp.y, cTmp.z, 1.0f};

        // multiply by the world matrix...
        a = world * a;
        b = world * b;
        c = world * c;

        // transform the vertices relative to the camera
        a = view * a;
        b = view * b;
        c = view * c;

        // backface culling
        vec3 vecA = Vec4ToVec3(a);
        vec3 ab = Vec4ToVec3(b) - vecA;
        vec3 ac = Vec4ToVec3(c) - vecA;
        vec3 normal = normalized(cross(ac, ab));
        vec3 origin = {0, 0, 0};
        vec3 cameraRay = origin - vecA;
        f32 normalDirection = dot(normal, cameraRay);
        if(normalDirection < 0.0f) {
            continue;
        }

        a = proj * a;
        b = proj * b;
        c = proj * c;
        
        Point aPoint = {((a.x / a.w) * 400) + 400, ((a.y / a.w) * 300) + 300, a.w};
        Point bPoint = {((b.x / b.w) * 400) + 400, ((b.y / b.w) * 300) + 300, b.w};
        Point cPoint = {((c.x / c.w) * 400) + 400, ((c.y / c.w) * 300) + 300, c.w};
        
        DrawTextureTriangle(renderer, aPoint, bPoint, cPoint, aUv, bUv, cUv, bitmap);
        // render DEBUG wireframe 
        DrawLineTriangle(renderer, aPoint, bPoint, cPoint, 0x0000FF00);
    }
}

internal 
void DrawLine(Renderer *renderer, Point a, Point b, u32 color) {
    i32 xDelta = (i32)(b.x - a.x);
    i32 yDelta = (i32)(b.y - a.y);
    i32 sideLength = abs(xDelta) >= abs(yDelta) ? abs(xDelta) : abs(yDelta);
    f32 xInc = (f32)xDelta / (f32)sideLength;
    f32 yInc = (f32)yDelta / (f32)sideLength;
    f32 x = a.x;
    f32 y = a.y;
    for(i32 i = 0; i < sideLength; ++i) {
        
        vec2 start = {a.x, a.y};
        vec2 delta = { b.x - a.x, b.y - a.y };
        vec2 p = { x, y };
        vec2 pRel = start - p;
        f32 t = len(pRel) / len(delta);
        f32 interpolatedReciprocalZ = ((1.0f/a.z) + ((1.0f/b.z) - (1.0f/a.z)) * t) + 0.001f; 
        if(interpolatedReciprocalZ >= renderer->depthBuffer[(i32)y * 800 + (i32)x]) {
            renderer->depthBuffer[(i32)y * 800 + (i32)x] = interpolatedReciprocalZ;
            renderer->colorBuffer[(i32)y * 800 + (i32)x] = color;
        } 
        x += xInc;
        y += yInc;
    }
}

internal
void DrawLineTriangle(Renderer *renderer, Point a, Point b, Point c, u32 color) {
    DrawLine(renderer, a, b, color);
    DrawLine(renderer, b, c, color);
    DrawLine(renderer, c, a, color);
}

internal
void DrawFillTriangle(Renderer *renderer, Point a, Point b, Point c, u32 color) {
    if(a.y > b.y) {
        SwapPoint(&a, &b);
    }
    if(b.y > c.y) {
        SwapPoint(&b, &c);
    }
    if(a.y > b.y) {
        SwapPoint(&a, &b);
    }
    i32 x0 = a.x;
    i32 y0 = a.y;
    i32 x1 = b.x;
    i32 y1 = b.y;
    i32 x2 = c.x;
    i32 y2 = c.y;
    f32 invSlope1 = 0;
    f32 invSlope2 = 0; 
    if(y1 - y0 != 0) invSlope1 = (f32)(x1 - x0) / abs((y1 - y0));
    if(y2 - y0 != 0) invSlope2 = (f32)(x2 - x0) / abs((y2 - y0));
    if(y1 - y0 != 0) {
        for(i32 y = y0; y < y1; y++) {
            i32 xStart = x1 + (y - y1) * invSlope1;
            i32 xEnd = x0 + (y - y0) * invSlope2;
            if(xEnd < xStart) {
                SwapInt(&xStart, &xEnd);
            }
            for(i32 x  = xStart; x <= xEnd; x++) {
                vec3 weights = SolveBarycentric({a.x, a.y}, {b.x, b.y}, {c.x, c.y}, {(f32)x, (f32)y});
                f32 interpolatedReciprocalZ = (1.0f/a.z) * weights.x + (1.0f/b.z) * weights.y + (1.0f/c.z) * weights.z;
                if(interpolatedReciprocalZ >= renderer->depthBuffer[(i32)y * 800 + (i32)x]) {
                    renderer->depthBuffer[(i32)y * 800 + (i32)x] = interpolatedReciprocalZ;
                    renderer->colorBuffer[(i32)y * 800 + (i32)x] = color;
                }
            }
        }
    }
    invSlope1 = 0;
    invSlope2 = 0;
    if(y2 - y1 != 0) invSlope1 = (f32)(x2 - x1) / abs(y2 - y1);
    if(y2 - y0 != 0) invSlope2 = (f32)(x2 - x0) / abs(y2 - y0);
    if(y2 - y1 != 0) {
        for(i32 y = y1; y < y2; y++) {
            i32 xStart = x1 + (y - y1) * invSlope1;
            i32 xEnd = x0 + (y - y0) * invSlope2;
            if(xEnd < xStart) {
                SwapInt(&xStart, &xEnd);
            }
            for(i32 x  = xStart; x <= xEnd; x++) {
                vec3 weights = SolveBarycentric({a.x, a.y}, {b.x, b.y}, {c.x, c.y}, {(f32)x, (f32)y});
                f32 interpolatedReciprocalZ = (1.0f/a.z) * weights.x + (1.0f/b.z) * weights.y + (1.0f/c.z) * weights.z; 
                if(interpolatedReciprocalZ >= renderer->depthBuffer[(i32)y * 800 + (i32)x]) {
                    renderer->depthBuffer[(i32)y * 800 + (i32)x] = interpolatedReciprocalZ;
                    renderer->colorBuffer[(i32)y * 800 + (i32)x] = color;
                }
            }
        }
    }
}

internal
void DrawTextureTriangle(Renderer *renderer, Point a, Point b, Point c,
                         vec2 aUv, vec2 bUv, vec2 cUv, Bitmap bitmap) {
    if(a.y > b.y) {
        SwapPoint(&a, &b);
        SwapVec2(&aUv, &bUv);
    }
    if(b.y > c.y) {
        SwapPoint(&b, &c);
        SwapVec2(&bUv, &cUv);
    }
    if(a.y > b.y) {
        SwapPoint(&a, &b);
        SwapVec2(&aUv, &bUv);
    }
    i32 x0 = a.x;
    i32 y0 = a.y;
    i32 x1 = b.x;
    i32 y1 = b.y;
    i32 x2 = c.x;
    i32 y2 = c.y;
    f32 invSlope1 = 0;
    f32 invSlope2 = 0;
    if(y1 - y0 != 0) invSlope1 = (f32)(x1 - x0) / abs((y1 - y0));
    if(y2 - y0 != 0) invSlope2 = (f32)(x2 - x0) / abs((y2 - y0));
    if(y1 - y0 != 0) {
        for(i32 y = y0; y < y1; y++) {
            i32 xStart = x1 + (y - y1) * invSlope1;
            i32 xEnd = x0 + (y - y0) * invSlope2;
            if(xEnd < xStart) {
                SwapInt(&xStart, &xEnd);
            }
            for(i32 x  = xStart; x <= xEnd; x++) {
                vec3 weights = SolveBarycentric({a.x, a.y}, {b.x, b.y}, {c.x, c.y}, {(f32)x, (f32)y});
                f32 interpolatedU = (aUv.x/a.z) * weights.x + (bUv.x/b.z) * weights.y + (cUv.x/c.z) * weights.z;
                f32 interpolatedV = (aUv.y/a.z) * weights.x + (bUv.y/b.z) * weights.y + (cUv.y/c.z) * weights.z;
                f32 interpolatedReciprocalZ = (1.0f/a.z) * weights.x + (1.0f/b.z) * weights.y + (1.0f/c.z) * weights.z;
                interpolatedU /= interpolatedReciprocalZ;
                interpolatedV /= interpolatedReciprocalZ;
                i32 bitmapX = abs((i32)(interpolatedU * bitmap.width)) % bitmap.width;
                i32 bitmapY = abs((i32)(interpolatedV * bitmap.height)) % bitmap.height;
                if(interpolatedReciprocalZ >= renderer->depthBuffer[(i32)y * 800 + (i32)x]) {
                    u32 color = ((u32 *)bitmap.data)[bitmapY * bitmap.width + bitmapX];
                    renderer->depthBuffer[(i32)y * 800 + (i32)x] = interpolatedReciprocalZ;
                    renderer->colorBuffer[(i32)y * 800 + (i32)x] = color;
                }
            }
        }
    }
    invSlope1 = 0;
    invSlope2 = 0;
    if(y2 - y1 != 0) invSlope1 = (f32)(x2 - x1) / abs(y2 - y1);
    if(y2 - y0 != 0) invSlope2 = (f32)(x2 - x0) / abs(y2 - y0);
    if(y2 - y1 != 0) {
        for(i32 y = y1; y < y2; y++) {
            i32 xStart = x1 + (y - y1) * invSlope1;
            i32 xEnd = x0 + (y - y0) * invSlope2;
            if(xEnd < xStart) {
                SwapInt(&xStart, &xEnd);
            }
            for(i32 x  = xStart; x <= xEnd; x++) {
                vec3 weights = SolveBarycentric({a.x, a.y}, {b.x, b.y}, {c.x, c.y}, {(f32)x, (f32)y});
                f32 interpolatedU = (aUv.x/a.z) * weights.x + (bUv.x/b.z) * weights.y + (cUv.x/c.z) * weights.z;
                f32 interpolatedV = (aUv.y/a.z) * weights.x + (bUv.y/b.z) * weights.y + (cUv.y/c.z) * weights.z;
                f32 interpolatedReciprocalZ = (1.0f/a.z) * weights.x + (1.0f/b.z) * weights.y + (1.0f/c.z) * weights.z;
                interpolatedU /= interpolatedReciprocalZ;
                interpolatedV /= interpolatedReciprocalZ;
                i32 bitmapX = abs((i32)(interpolatedU * bitmap.width)) % bitmap.width;
                i32 bitmapY = abs((i32)(interpolatedV * bitmap.height)) % bitmap.height;
                if(interpolatedReciprocalZ >= renderer->depthBuffer[(i32)y * 800 + (i32)x]) {
                    u32 color = ((u32 *)bitmap.data)[bitmapY * bitmap.width + bitmapX];
                    renderer->depthBuffer[(i32)y * 800 + (i32)x] = interpolatedReciprocalZ;
                    renderer->colorBuffer[(i32)y * 800 + (i32)x] = color;
                }
            }
        }
    }
}


internal
void DrawPoint(Renderer *renderer, Point point, u32 color) {
    i32 x = (f32)point.x;
    i32 y = (f32)point.y;
    if(x >= 0 && x <= 800 && y >= 0 && y <= 600) {
        renderer->colorBuffer[(i32)y * 800 + (i32)x] = color;
    }
}


