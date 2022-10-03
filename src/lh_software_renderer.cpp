internal
void RenderBuffer(u32 *buffer, vec3 *vertices, i32 verticesCount) {

// TODO: render the vertices list from a camera point of view
    for(i32 i = 0; i < verticesCount; i += 3) {
        vec3 aTmp = vertices[i + 0];
        vec3 bTmp = vertices[i + 1];
        vec3 cTmp = vertices[i + 2];

        vec4 a = {aTmp.x, aTmp.y, aTmp.z, 1.0f};
        vec4 b = {bTmp.x, bTmp.y, bTmp.z, 1.0f};
        vec4 c = {cTmp.x, cTmp.y, cTmp.z, 1.0f};

        // TODO: multiply by the world matrix...
        mat4 view = Mat4LookAt({100, 0, -8}, {100, 0, 0}, {0, 1, 0}); 
        //mat4 proj = Mat4Perspective(60.0f, 800.0f/600.0f, 0.1f, 100.0f);
        mat4 proj = Mat4Ortho(-400, 400, -300, 300, 0.1f, 100.0f);
        OutputDebugString("View:\n");
        Mat4Print(view);
        OutputDebugString("Proj:\n");
        Mat4Print(proj);
        // transform the vertices relative to the camera
        a = view * a;
        b = view * b;
        c = view * c;

        a = proj * a;
        b = proj * b;
        c = proj * c;
        
        Point aPoint = {((a.x / a.w) * 400) + 400, ((a.y / a.w) * 300) + 300};
        Point bPoint = {((b.x / b.w) * 400) + 400, ((b.y / b.w) * 300) + 300};
        Point cPoint = {((c.x / c.w) * 400) + 400, ((c.y / c.w) * 300) + 300};
        DrawFillTriangle(buffer, aPoint, bPoint, cPoint, 0x00FF0000);

        // render DEBUG wireframe 
        DrawLineTriangle(buffer, aPoint, bPoint, cPoint, 0x0000FF00);

    }
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
void DrawLine(u32 *buffer, Point a, Point b, u32 color) {
    i32 xDelta = (i32)(b.x - a.x);
    i32 yDelta = (i32)(b.y - a.y);
    i32 sideLength = abs(xDelta) >= abs(yDelta) ? abs(xDelta) : abs(yDelta);
    f32 xInc = (f32)xDelta / (f32)sideLength;
    f32 yInc = (f32)yDelta / (f32)sideLength;
    f32 x = a.x;
    f32 y = a.y;
    for(i32 i = 0; i < sideLength; ++i) {
        buffer[(i32)y * 800 + (i32)x] = color;
        x += xInc;
        y += yInc;
    }
}

internal
void DrawLineTriangle(u32 *buffer, Point a, Point b, Point c, u32 color) {
    DrawLine(buffer, a, b, color);
    DrawLine(buffer, b, c, color);
    DrawLine(buffer, c, a, color);
}

internal
void DrawFillTriangle(u32 *buffer, Point a, Point b, Point c, u32 color) {
    if(a.y > b.y) {
        SwapVec2(&a, &b);
    }
    if(b.y > c.y) {
        SwapVec2(&b, &c);
    }
    if(a.y > b.y) {
        SwapVec2(&a, &b);
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
                buffer[(i32)y * 800 + (i32)x] = color;
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
                buffer[(i32)y * 800 + (i32)x] = color;
            }
        }
    }
}

internal
void DrawPoint(u32 *buffer, Point point, u32 color) {
    i32 x = (f32)point.x;
    i32 y = (f32)point.y;
    if(x >= 0 && x <= 800 && y >= 0 && y <= 600) {
        buffer[(i32)y * 800 + (i32)x] = color;
    }
}


