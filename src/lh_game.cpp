#include "lh_game.h"
#include "lh_platform.h"
#include "lh_renderer.h"
#include "lh_sound.h"
#include "lh_texture.h"
#include "lh_input.h"

//////////////////////////////////////////////////////////////////////
// TODO (manuto):
//////////////////////////////////////////////////////////////////////
// - add mutiple textures to static entities
// - add texture atlas
// - fix lights
// - add mutiple lights to the renderer
// - platform independent Debug output
// - add REAL DEBUGING geometry 
//////////////////////////////////////////////////////////////////////

Vertex verticesCube[] = {
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f,  0.0f,  1.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f,  0.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,-1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,-1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,-1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,-1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,-1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,-1.0f,  0.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f
};

Vertex vertices[] = {
    // position           // uv        // normal
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f
};

u32 indices[] = {
    0, 1, 3,
    3, 1, 2
};


// TODO: FPS camera
vec3 cameraPosition = {2, 5, 1};
vec3 cameraLastPosition = {2, 5, 1};
vec3 cameraFront = {0, 0, 1};
vec3 cameraRight = {1, 0, 0};
vec3 cameraUp = {0, 1, 0};
f32 cameraPitch = 0;
f32 cameraYaw = RAD(90.0f);
bool cameraIsColliding = false;
Capsule cameraCapsule;

f32 playerSpeed = 3.0f;
f32 sensitivity = 2.0f;
f32 mouseSensitivity = 0.001f;

#include <windows.h>
#include <stdio.h>

internal
void UpdateCamera(f32 dt, GameState *gameState) {
    f32 leftStickX = JoysickGetLeftStickX();
    f32 leftStickY = JoysickGetLeftStickY();
    f32 rightStickX = JoysickGetRightStickX();
    f32 rightStickY = JoysickGetRightStickY();
    
    if(MouseGetButtonJustDown(MOUSE_BUTTON_RIGHT)) {
        MouseShowCursor(false);
        gameState->mouseDefaultScreenX = MouseGetScreenX();
        gameState->mouseDefaultScreenY = MouseGetScreenY();
    }
    if(MouseGetButtonJustUp(MOUSE_BUTTON_RIGHT)) {
        MouseShowCursor(true);
    }
    if(MouseGetButtonDown(MOUSE_BUTTON_RIGHT)) {
        f32 deltaMouseX = (f32)(MouseGetScreenX() - gameState->mouseDefaultScreenX);
        f32 deltaMouseY = (f32)(MouseGetScreenY() - gameState->mouseDefaultScreenY);
        MouseSetCursor(gameState->mouseDefaultScreenX, gameState->mouseDefaultScreenY);
        cameraYaw -= (deltaMouseX * mouseSensitivity);
        cameraPitch -= (deltaMouseY * mouseSensitivity); 
    }
    
    // Right Stick movement
    cameraYaw -= (rightStickX * sensitivity) * dt;
    cameraPitch += (rightStickY * sensitivity) * dt;

    f32 maxPitch = RAD(89.0f);
    if(cameraPitch > maxPitch) {
        cameraPitch = maxPitch;
    }
    else if(cameraPitch < -maxPitch) {
        cameraPitch = -maxPitch;
    }
    cameraFront.x = cosf(cameraYaw) * cosf(cameraPitch);
    cameraFront.y = sinf(cameraPitch);
    cameraFront.z = sinf(cameraYaw) * cosf(cameraPitch);
    cameraRight = cross(cameraUp, cameraFront);


    if(KeyboardGetKeyDown(KEYBOARD_KEY_W)) {
        cameraPosition = cameraPosition + (cameraFront * playerSpeed) * dt;
    }
    if(KeyboardGetKeyDown(KEYBOARD_KEY_S)) {
        cameraPosition = cameraPosition - (cameraFront * playerSpeed) * dt;
    }
    if(KeyboardGetKeyDown(KEYBOARD_KEY_D)) {
        cameraPosition = cameraPosition + (cameraRight * playerSpeed) * dt;
    }
    if(KeyboardGetKeyDown(KEYBOARD_KEY_A)) {
        cameraPosition = cameraPosition - (cameraRight * playerSpeed) * dt;
    }

    // Left Stick movement
    cameraPosition = cameraPosition + (cameraRight * (leftStickX * playerSpeed)) * dt;
    cameraPosition = cameraPosition + (cameraFront * (leftStickY * playerSpeed)) * dt;    

    cameraCapsule.a = cameraPosition;
    cameraCapsule.a.y += 0.2f;
    cameraCapsule.b = cameraPosition;
    cameraCapsule.b.y -= 0.6f;
}

// TODO: map test

const i32 mapCountX = 16;
const i32 mapCountY = 16;
i32 map[mapCountY][mapCountX] = {
    0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 1, 4, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0,
    2, 1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0,
    2, 1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0,
    2, 1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 3, 3, 0,
    2, 1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,
    2, 1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 8, 5, 9, 1, 4,
    2, 1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 4, 0, 2, 1, 4,
    2, 1, 1, 6, 7, 1, 1, 1, 8, 5, 5, 0, 0, 2, 1, 4,
    2, 1, 1, 1, 1, 1, 1, 8, 0, 0, 0, 0, 0, 2, 1, 4,
    2, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, 2, 1, 4,
    2, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, 0, 5, 0,
    2, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

internal
mat4 TransformToMat4(vec3 position, vec3 rotation, vec3 scale) {
    mat4 translationMat = Mat4Translate(position.x, position.y, position.z);
    mat4 rotationX = Mat4RotateX(RAD(rotation.x));
    mat4 rotationY = Mat4RotateY(RAD(rotation.y));
    mat4 rotationZ = Mat4RotateZ(RAD(rotation.z));
    mat4 scaleMat = Mat4Scale(scale.x, scale.y, scale.z);
    mat4 rotationMat = rotationX * rotationY * rotationZ;
    mat4 world = translationMat * rotationMat * scaleMat;
    return world;
}

OBB CreateOBB(vec3 position, vec3 rotation, vec3 scale) {
    // Init OBB
    OBB result = {};
    vec3 cubeOBBRight = {1, 0, 0};
    vec3 cubeOBBUp    = {0, 1, 0};
    vec3 cubeOBBFront = {0, 0, 1};
    result.c = position;
    result.e = scale;
    cubeOBBRight = Mat3RotateX(RAD(rotation.x)) * cubeOBBRight;
    cubeOBBUp    = Mat3RotateX(RAD(rotation.x)) * cubeOBBUp;
    cubeOBBFront = Mat3RotateX(RAD(rotation.x)) * cubeOBBFront;
    cubeOBBRight = Mat3RotateY(RAD(rotation.y)) * cubeOBBRight;
    cubeOBBUp    = Mat3RotateY(RAD(rotation.y)) * cubeOBBUp;
    cubeOBBFront = Mat3RotateY(RAD(rotation.y)) * cubeOBBFront;
    cubeOBBRight = Mat3RotateZ(RAD(rotation.z)) * cubeOBBRight;
    cubeOBBUp    = Mat3RotateZ(RAD(rotation.z)) * cubeOBBUp;
    cubeOBBFront = Mat3RotateZ(RAD(rotation.z)) * cubeOBBFront;
    result.u[0] = normalized(cubeOBBRight);
    result.u[1] = normalized(cubeOBBUp);
    result.u[2] = normalized(cubeOBBFront);
    mat4 translationMat = Mat4Translate(position.x, position.y, position.z);
    mat4 rotationMat = Mat4RotateX(RAD(rotation.x)) * Mat4RotateY(RAD(rotation.y)) * Mat4RotateZ(RAD(rotation.z));
    mat4 scaleMat =  Mat4Scale(scale.x, scale.y, scale.z);
    result.world = translationMat * rotationMat  *scaleMat;
    result.color = 0xFF00FF00;
    return result;
}

#include "lh_map.h"

internal
vec3 ClosestPtPointPlane(vec3 q, Plane plane)
{
    vec3 n = plane.n;
    vec3 p = plane.p;
    f32 t = dot(n, q - p) / dot(n , n);
    vec3 r = q - (n * t);
    return r;
}

internal
vec3 ClosestPtPointOBB(vec3 p, OBB *b) {
    vec3 result = b->c;
    vec3 d = p - b->c;
    // for each OBB axis...
    for(i32 i = 0; i < 3; ++i) {
        // ...project d onto that axis to get the distance
        // along the axis of d from the box center
        f32 dist = dot(d, b->u[i]);
       // if dist farther than the box extents clamp it
       if(dist > b->e.v[i]) dist = b->e.v[i];
       if(dist < -b->e.v[i]) dist = -b->e.v[i];
       // step that distance along the axis to get the world coord
       result = result + b->u[i] * dist;
    }
    return result;
}

internal
f32 SqDistPointOBB(vec3 p, OBB *b) {
    vec3 v = p - b->c;
    f32 sqDist = 0.0f;
    for(i32 i = 0; i < 3; ++i) {
        // Project vector from box center to p on each axis, getting the distance
        // of p along that axis, and count any excess distance outside box extends
        f32 d = dot(v, b->u[i]), excess = 0.0f;
        if(d < -b->e.v[i]) {
            excess = d + b->e.v[i];
        }
        else if (d > b->e.v[i]) {
            excess = d - b->e.v[i];
        }
        sqDist += excess * excess;
    }
    return sqDist;
}

internal
i32 TestOBBOBB(OBB *a, OBB *b) {
    f32 ra, rb;
    mat3 R, AbsR;

    // compute rotation matrix expressing b in a's coordinate frame
    for(i32 i = 0; i < 3; ++i) {
        for(i32 j = 0; j < 3; ++j) {
            R.m[i * 3 + j] = dot(a->u[i], b->u[j]);
        }
    }
    // compute translation vector t
    vec3 t = b->c - a->c;
    // bring translation into a's coordinate frame
    t = {dot(t, a->u[0]), dot(t, a->u[1]), dot(t, a->u[2])};
    // compute common subexpressions. add in an epsilon term to
    // counteract arithmetic errors when two edges are parallel and
    // their cross product is near null
    for(i32 i = 0; i < 3; ++i) {
        for(i32 j = 0; j < 3; ++j) {
            AbsR.m[i * 3 + j] = fabsf(R.m[i * 3 + j]) + EPSILON;
        }
    }
    // Test axes L = AO, L =  A1, L = A2
    for(i32 i = 0; i < 3; ++i) {
        ra = a->e.v[i];
        rb = b->e.v[0] * AbsR.m[i * 3 + 0] + b->e.v[1] * AbsR.m[i * 3 + 1] + b->e.v[2] * AbsR.m[i * 3 + 2];
        if(fabsf(t.v[i]) > ra + rb) return 0;
    }
    // Test axes L = AO, L =  A1, L = A2
    for(i32 i = 0; i < 3; ++i) {
        ra = a->e.v[0] * AbsR.m[0 * 3 + i] + a->e.v[1] * AbsR.m[1 * 3 + i] + a->e.v[2] * AbsR.m[2 * 3 + i];
        rb = b->e.v[i];
        if(fabsf(t.v[0] * R.m[0 * 3 + i] + t.v[1] * R.m[1 * 3 + i] + t.v[2] * R.m[2 * 3 + i]) > ra + rb) return 0;
    }
    
    // test axis L = AO x BO
    ra =  a->e.v[1] * AbsR.m[2 * 3 + 0] + a->e.v[2] * AbsR.m[1 * 3 + 0];
    rb =  b->e.v[1] * AbsR.m[0 * 3 + 2] + b->e.v[2] * AbsR.m[0 * 3 + 1];
    if(fabsf(t.v[2] *    R.m[1 * 3 + 0] -    t.v[1] *    R.m[2 * 3 + 0]) > ra + rb) return 0;

    // test axis L = AO x B1
    ra =  a->e.v[1] * AbsR.m[2 * 3 + 1] + a->e.v[2] * AbsR.m[1 * 3 + 1];
    rb =  b->e.v[0] * AbsR.m[0 * 3 + 2] + b->e.v[2] * AbsR.m[0 * 3 + 0];
    if(fabsf(t.v[2] *    R.m[1 * 3 + 1] -    t.v[1] *    R.m[2 * 3 + 1]) > ra + rb) return 0;

    // test axis L = AO x B2
    ra =  a->e.v[1] * AbsR.m[2 * 3 + 2] + a->e.v[2] * AbsR.m[1 * 3 + 2];
    rb =  b->e.v[0] * AbsR.m[0 * 3 + 1] + b->e.v[1] * AbsR.m[0 * 3 + 0];
    if(fabsf(t.v[2] *    R.m[1 * 3 + 2] -    t.v[1] *    R.m[2 * 3 + 2]) > ra + rb) return 0;

    // test axis L = A1 x B0
    ra =  a->e.v[0] * AbsR.m[2 * 3 + 0] + a->e.v[2] * AbsR.m[0 * 3 + 0];
    rb =  b->e.v[1] * AbsR.m[1 * 3 + 2] + b->e.v[2] * AbsR.m[1 * 3 + 1];
    if(fabsf(t.v[0] *    R.m[2 * 3 + 0] -    t.v[2] *    R.m[0 * 3 + 0]) > ra + rb) return 0;

    // test axis L = A1 x B1
    ra =  a->e.v[0] * AbsR.m[2 * 3 + 1] + a->e.v[2] * AbsR.m[0 * 3 + 1];
    rb =  b->e.v[0] * AbsR.m[1 * 3 + 2] + b->e.v[2] * AbsR.m[1 * 3 + 0];
    if(fabsf(t.v[0] *    R.m[2 * 3 + 1] -    t.v[2] *    R.m[0 * 3 + 1]) > ra + rb) return 0;

    // test axis L = A1 x B2
    ra =  a->e.v[0] * AbsR.m[2 * 3 + 2] + a->e.v[2] * AbsR.m[0 * 3 + 2];
    rb =  b->e.v[0] * AbsR.m[1 * 3 + 1] + b->e.v[1] * AbsR.m[1 * 3 + 0];
    if(fabsf(t.v[0] *    R.m[2 * 3 + 2] -    t.v[2] *    R.m[0 * 3 + 2]) > ra + rb) return 0;

    // test axis L = A2 x B0
    ra =  a->e.v[0] * AbsR.m[1 * 3 + 0] + a->e.v[1] * AbsR.m[0 * 3 + 0];
    rb =  b->e.v[1] * AbsR.m[2 * 3 + 2] + b->e.v[2] * AbsR.m[2 * 3 + 1];
    if(fabsf(t.v[1] *    R.m[0 * 3 + 0] -    t.v[0] *    R.m[1 * 3 + 0]) > ra + rb) return 0;

    // test axis l = A2 X B1
    ra =  a->e.v[0] * AbsR.m[1 * 3 + 1] + a->e.v[1] * AbsR.m[0 * 3 + 1];
    rb =  b->e.v[0] * AbsR.m[2 * 3 + 2] + b->e.v[2] * AbsR.m[2 * 3 + 0];
    if(fabsf(t.v[1] *    R.m[0 * 3 + 1] -    t.v[0] *    R.m[1 * 3 + 1]) > ra + rb) return 0;

    // test axis L = A2 x B1
    ra =  a->e.v[0] * AbsR.m[1 * 3 + 2] + a->e.v[1] * AbsR.m[0 * 3 + 2];
    rb =  b->e.v[0] * AbsR.m[2 * 3 + 1] + b->e.v[1] * AbsR.m[2 * 3 + 0];
    if(fabsf(t.v[1] *    R.m[0 * 3 + 2] -    t.v[0] *    R.m[1 * 3 + 2]) > ra + rb) return 0;
    
    return 1;
}

f32 SqDistPointSegment(vec3 a, vec3 b, vec3 c) {
    vec3 ab = b - a;
    vec3 ac = c - a;
    vec3 bc = c - b;
    f32 e = dot(ac, ab);
    // Handle cases where c projects outside ab
    if (e <= 0.0f) return dot(ac, ac);
    f32 f = dot(ab, ab);
    if (e >= f) return dot(bc, bc);
    // Handle cases where c projects onto ab
    return dot(ac, ac) - e * e / f;
}

vec3 ClosestPtPointSegment(vec3 c, vec3 a, vec3 b) {
    vec3 ab = b - a;
    f32 t = dot(c - a, ab) / dot(ab, ab);
    if(t < 0.0f) t = 0.0f;
    if(t > 1.0f) t = 1.0f;
    vec3 d = a + ab * t;
    return d;
}

void TestCameraCapsuleOBBsArray(StaticEntityNode *entities,i32 count) {
    i32 collisionCount = 0;
    vec3 lastCollisionResult;
    for(i32 i = 0; i < count; ++i) {
        StaticEntityNode *entityNode = entities + i;
        StaticEntity *staticEntity = entityNode->object;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            OBB *obb = staticEntity->obbs + j;
            vec3 closestPoint = ClosestPtPointOBB(cameraPosition, obb);
            vec3 testPosition = ClosestPtPointSegment(closestPoint, cameraCapsule.a, cameraCapsule.b);
            f32 distanceSq = lenSq(closestPoint - testPosition);
            if(distanceSq > cameraCapsule.r * cameraCapsule.r) {
                obb->color = 0xFF00FF00;
                continue;
            }
            obb->color = 0xFFFF0000;
            vec3 normal = {0, 0, 1};
            if(CMP(distanceSq, 0.0f)) {
                f32 mSq = lenSq(closestPoint - obb->c);
                if(CMP(mSq, 0.0f)) {
                }
                else {
                    normal = normalized(closestPoint - obb->c);
                }
            }
            else {
                normal = normalized(testPosition - closestPoint);
            }

#if 0
            vec3 projectionX = project(normal, obb->u[0]);
            vec3 projectionY = project(normal, obb->u[1]);
            vec3 projectionZ = project(normal, obb->u[2]);
            f32 projectionSqLenX = lenSq(projectionX);
            f32 projectionSqLenY = lenSq(projectionY);
            f32 projectionSqLenZ = lenSq(projectionZ);
            if(projectionSqLenY > projectionSqLenX && projectionSqLenY > projectionSqLenZ) {
                normal = normalized(projectionY);
            }
            else if(projectionSqLenX > projectionSqLenY && projectionSqLenX > projectionSqLenZ) {
                normal = normalized(projectionX);
            }
            else if(projectionSqLenZ > projectionSqLenX && projectionSqLenZ > projectionSqLenY) {
                normal = normalized(projectionZ);
            }
            normal = {0, 1, 0};
#endif
            if(collisionCount == 2) {
                i32 stopHere = 0;
            }

            vec3 outsidePoint = testPosition - normal * cameraCapsule.r;
            f32 distance = len(closestPoint - outsidePoint);
            cameraPosition = cameraPosition + (normal * distance);
            lastCollisionResult = cameraPosition;
            cameraCapsule.a = cameraPosition;
            cameraCapsule.a.y += 0.2f;
            cameraCapsule.b = cameraPosition;
            cameraCapsule.b.y -= 0.6f;
            collisionCount++;
        }
    }
}


void TestCameraCapsuleOBB(OBB *obb) {
    vec3 closestPoint = ClosestPtPointOBB(cameraPosition, obb);
    vec3 testPosition = ClosestPtPointSegment(closestPoint, cameraCapsule.a, cameraCapsule.b);
    f32 distanceSq = lenSq(closestPoint - testPosition);
    if(distanceSq > cameraCapsule.r * cameraCapsule.r) {
        obb->color = 0xFF00FF00;
        return;
    }
    obb->color = 0xFFFF0000;
    vec3 normal = {0, 0, 0};
    if(CMP(distanceSq, 0.0f)) {
        f32 mSq = lenSq(closestPoint - obb->c);
        if(CMP(mSq, 0.0f)) {
        }
        else {
            normal = normalized(closestPoint - obb->c);
        }
    }
    else {
        normal = normalized(testPosition - closestPoint);
    }
    
    vec3 outsidePoint = testPosition - normal * cameraCapsule.r;
    f32 distance = len(closestPoint - outsidePoint);
    cameraPosition = cameraPosition + normal * distance;
    cameraCapsule.a = cameraPosition;
    cameraCapsule.a.y += 0.2f;
    cameraCapsule.b = cameraPosition;
    cameraCapsule.b.y -= 0.6f;
}


void TestCameraSphereOBBsArray(StaticEntityNode *entities,i32 count) {
    for(i32 i = 0; i < count; ++i) {
        StaticEntityNode *entityNode = entities + i;
        StaticEntity *staticEntity = entityNode->object;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            OBB *obb = staticEntity->obbs + j;
            vec3 closestPoint = ClosestPtPointOBB(cameraPosition, obb);
            f32 distanceSq = lenSq(closestPoint - cameraPosition);
            if(distanceSq > cameraCapsule.r * cameraCapsule.r) {
                obb->color = 0xFF00FF00;
                continue;
            }
            obb->color = 0xFFFF0000;
            vec3 normal = {0, 1, 0};
            if(CMP(distanceSq, 0.0f)) {
                f32 mSq = lenSq(closestPoint - obb->c);
                if(CMP(mSq, 0.0f)) {
                }
                else {
                    normal = normalized(closestPoint - obb->c);
                }
            }
            else {
                normal = normalized(cameraPosition - closestPoint);
            }
            vec3 projectionX = project(normal, obb->u[0]);
            vec3 projectionY = project(normal, obb->u[1]);
            vec3 projectionZ = project(normal, obb->u[2]);
            f32 projectionSqLenX = lenSq(projectionX);
            f32 projectionSqLenY = lenSq(projectionY);
            f32 projectionSqLenZ = lenSq(projectionZ);
            if(projectionSqLenY > projectionSqLenX && projectionSqLenY > projectionSqLenZ) {
                normal = normalized(projectionY);
            }
            else if(projectionSqLenX > projectionSqLenY && projectionSqLenX > projectionSqLenZ) {
                normal = normalized(projectionX);
            }
            else if(projectionSqLenZ > projectionSqLenX && projectionSqLenZ > projectionSqLenY) {
                normal = normalized(projectionZ);
            }
#if 1
            Plane collisionPlane; 
            collisionPlane.n = normal;
            collisionPlane.p = closestPoint;
            cameraPosition = ClosestPtPointPlane(cameraPosition, collisionPlane) +
                                              (collisionPlane.n * cameraCapsule.r) + 
                                              (collisionPlane.n * 0.002f);
#else
            vec3 outsidePoint = cameraPosition - normal * cameraCapsule.r;
            f32 distance = len(closestPoint - outsidePoint);
            cameraPosition = cameraPosition + (normal * distance) + (normal * 0.002f);
#endif


        }
    }
}

void TestCameraSphereOBB(OBB *obb) {
    vec3 closestPoint = ClosestPtPointOBB(cameraPosition, obb);
    f32 distanceSq = lenSq(closestPoint - cameraPosition);
    if(distanceSq > cameraCapsule.r * cameraCapsule.r) {
        obb->color = 0xFF00FF00;
        return;
    }
    obb->color = 0xFFFF0000;
    vec3 normal = {0, 1, 0};
    if(CMP(distanceSq, 0.0f)) {
        f32 mSq = lenSq(closestPoint - obb->c);
        if(CMP(mSq, 0.0f)) {
        }
        else {
            normal = normalized(closestPoint - obb->c);
        }
    }
    else {
        normal = normalized(cameraPosition - closestPoint);
    }
    vec3 projectionX = project(normal, obb->u[0]);
    vec3 projectionY = project(normal, obb->u[1]);
    vec3 projectionZ = project(normal, obb->u[2]);
    f32 projectionSqLenX = lenSq(projectionX);
    f32 projectionSqLenY = lenSq(projectionY);
    f32 projectionSqLenZ = lenSq(projectionZ);
    if(projectionSqLenY > projectionSqLenX && projectionSqLenY > projectionSqLenZ) {
        normal = normalized(projectionY);
    }
    else if(projectionSqLenX > projectionSqLenY && projectionSqLenX > projectionSqLenZ) {
        normal = normalized(projectionX);
    }
    else if(projectionSqLenZ > projectionSqLenX && projectionSqLenZ > projectionSqLenY) {
        normal = normalized(projectionZ);
    }
#if 1
    Plane collisionPlane; 
    collisionPlane.n = normal;
    collisionPlane.p = closestPoint;
    cameraPosition = ClosestPtPointPlane(cameraPosition, collisionPlane) +
                                      (collisionPlane.n * cameraCapsule.r) + 
                                      (collisionPlane.n * 0.002f);
#else
    vec3 outsidePoint = cameraPosition - normal * cameraCapsule.r;
    f32 distance = len(closestPoint - outsidePoint);
    cameraPosition = cameraPosition + (normal * distance) + (normal * 0.002f);
#endif
}


bool RaycastOBB(OBB *obb, Ray *ray, f32 *tOut) {
    vec3 x = obb->u[0];
    vec3 y = obb->u[1];
    vec3 z = obb->u[2];
    vec3 p = obb->c - ray->o;
    vec3 f = {
        dot(x, ray->d),
        dot(y, ray->d),
        dot(z, ray->d)
    };
    vec3 e = {
        dot(x, p),
        dot(y, p),
        dot(z, p)
    };
    f32 t[6] = {0, 0, 0, 0, 0, 0};
    for(i32 i = 0; i < 3; ++i) {
        if(CMP(f.v[i], 0)) {
            if(-e.v[i] - obb->e.v[i] > 0 || -e.v[i] + obb->e.v[i] < 0) {
                return false;
            }
            f.v[i] = 0.00001f; // Avoid div by 0
        }
        t[(i * 2) + 0] = ((e.v[i] + obb->e.v[i]) / f.v[i]);
        t[(i * 2) + 1] = ((e.v[i] - obb->e.v[i]) / f.v[i]);
    }
    f32 tmin = fmaxf(fmaxf(fminf(t[0], t[1]),
                           fminf(t[2], t[3])),
                           fminf(t[4], t[5]));
    f32 tmax = fminf(fminf(fmaxf(t[0], t[1]),
                           fmaxf(t[2], t[3])),
                           fmaxf(t[4], t[5]));
    if(tmax < 0) {
        return false;
    }
    if(tmin > tmax) {
        return false;
    }
    if(tmin < 0) {
        *tOut = tmax;
        return true;
    }
    *tOut = tmin;
    return true;
}


f32 Clamp(f32 n, f32 min, f32 max) {
    if(n < min) return min;
    if(n > max) return max;
    return n;
}

f32 ClosestPtSegmentSegment(vec3 p1, vec3 q1,
                            vec3 p2, vec3 q2,
                            f32 &s, f32 &t,
                            vec3 &c1, vec3 &c2) {
    vec3 d1 = q1 - p1; // direction vector of segment s1
    vec3 d2 = q2 - p2; // direction vector of segment s2
    vec3 r = p1 - p2;
    f32 a = dot(d1, d1); // squared length of segment S1
    f32 e = dot(d2, d2); // squared length of segment S2
    f32 f = dot(d2, r);
    // check if either or both segments degenerate into points
    if(a <= EPSILON && e <= EPSILON) { 
        // Both segments degenerate into points
        s = t = 0.0f;
        c1 = p1;
        c2 = p2;
        return dot(c1 - c2, c1 - c2);
    }
    if(a <= EPSILON) {
        // first segment degenerate into a point
        s = 0.0f;
        t = f / e;
        t = Clamp(t, 0.0f, 1.0f); 
    }
    else {
        f32 c = dot(d1, r);
        if(e <= EPSILON) {
            // second segment degenerate into a point
            t = 0.0f;
            s = Clamp(-c / a, 0.0f, 1.0f);
        }
        else {
            // the general nondegenerate case start here
            f32 b = dot(d1, d2);
            f32 denom = a*e-b*b;
            // if segments not parallel, compute closest point on L1 to L2 and
            // clamp to segment S1. Else pick arbitrary s (here 0)
            if(denom != 0.0f) {
                s = Clamp((b*f - c*e) / denom, 0.0f, 1.0f); 
            }
            else s = 0.0f;
            // compute point on L2 closest to S1(s) using
            // t = dot((P1 + D1*s) - P2, D2) / dot(D2, D2) = (b*s + f) / e
            t = (b*s + f) / e;

            if(t < 0.0f) {
                t = 0.0f;
                s = Clamp(-c / a, 0.0f, 1.0f);
            }
            else if(t > 1.0f) {
                t = 1.0f;
                s = Clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }
    c1 = p1 + d1 * s;
    c2 = p2 + d2 * t;
    return dot(c1 - c2, c1 - c2);
}

bool IntersectSegmentCapsule(Segment seg, vec3 a, vec3 b, f32 radii, f32 *tOut) {
    f32 s = 0;
    f32 t = 0;
    vec3 c1 = {};
    vec3 c2 = {};
    f32 sqDist = ClosestPtSegmentSegment(seg.a, seg.b, a, b, s, t, c1, c2);
    f32 movementLength = len(seg.b - seg.a);
    *tOut = s;
    bool result = (sqDist <= radii*radii);
    return result;
}

vec3 Corner(OBB b, i32 n) {
    vec3 p = b.c;
    if(n & 1) {
        p = p + b.u[0] * b.e.x;
    }
    else {
        p = p - b.u[0] * b.e.x; 
    }
    if(n & 2) {
        p = p + b.u[1] * b.e.y;
    }
    else {
        p = p - b.u[1] * b.e.y; 
    }
    if(n & 4) {
        p = p + b.u[2] * b.e.z;
    }
    else {
        p = p - b.u[2] * b.e.z; 
    }

    i32 stopHere = 0;
    return p;
}

int IntersectMovingSphereOBB(Sphere s, vec3 d, OBB b, f32 *t) {
    // Compute the OBB resulting from expanding b by sphere radius r
    OBB e = b;
    e.e.x += s.r - 0.002f;
    e.e.y += s.r - 0.002f;
    e.e.z += s.r - 0.002f;
    // Intersect ray against expanded OBB e. Exit with no intersection if ray
    // misses e, else get intersection point p and time t as result
    vec3 p;
    Ray ray;
    ray.o = s.c;
    ray.d = d;
    if (!RaycastOBB(&e, &ray, t) || (*t) > 1.0f) {
        return 0;
    }

    // Compute which min and max faces of b the intersection point p lies
    // outside of. Note, u and v cannot have the same bits set and
    // they must have at least one bit set among them
    p = s.c + (d * (*t));
    
    vec3 pRel = p - b.c;
    f32 px = dot(pRel, b.u[0]);
    f32 py = dot(pRel, b.u[1]);
    f32 pz = dot(pRel, b.u[2]);
    
    i32 u = 0, v = 0;
    if(px < -b.e.x) u |= 1;
    if(px >  b.e.x) v |= 1;
    if(py < -b.e.y) u |= 2;
    if(py >  b.e.y) v |= 2;
    if(pz < -b.e.z) u |= 4;
    if(pz >  b.e.z) v |= 4;

    // ‘Or’ all set bits together into a bit mask (note: here u + v == u | v)
    i32 m = u + v;
    // Define line segment [c, c+d] specified by the sphere movement
    Segment seg = {s.c, s.c + d};
    // If all 3 bits set (m == 7) then p is in a vertex region
    if (m == 7) {

        i32 stopHere = 0;
        // Must now intersect segment [c, c+d] against the capsules of the three
        // edges meeting at the vertex and return the best time, if one or more hit
        float tmin = FLT_MAX;
        if (IntersectSegmentCapsule(seg, Corner(b, v), Corner(b, v ^ 1), s.r, t))
            tmin = fminf(*t, tmin);
        if (IntersectSegmentCapsule(seg, Corner(b, v), Corner(b, v ^ 2), s.r, t))
            tmin = fminf(*t, tmin);
        if (IntersectSegmentCapsule(seg, Corner(b, v), Corner(b, v ^ 4), s.r, t))
            tmin = fminf(*t, tmin);
        if (tmin == FLT_MAX) return 0;
            // No intersection
            *t = tmin;
        return 1; // Intersection at time t == tmin
    }

    // If only one bit set in m, then p is in a face region
    if ((m & (m - 1)) == 0) {
        // Do nothing. Time t from intersection with
        // expanded box is correct intersection time
        i32 stopHere = 0;
        return 1;
    }
    // p is in an edge region. Intersect against the capsule at the edge
    return IntersectSegmentCapsule(seg, Corner(b, u ^ 7), Corner(b, v), s.r, t);
}

void TestCameraSphereOBBDynamic(OBB *obb) {

    Sphere sphere = {};
    sphere.c = cameraLastPosition;
    sphere.r = cameraCapsule.r;
    vec3 d = cameraPosition - cameraLastPosition;

    f32 t = 0.0f;
    if(IntersectMovingSphereOBB(sphere, d, *obb, &t)) {
        i32 stopHere = 0;
        obb->color = 0xFFFF0000;
        Plane collisionPlane;
        vec3 closest = ClosestPtPointOBB(cameraLastPosition, obb);
        collisionPlane.n = normalized(cameraLastPosition - closest);
        collisionPlane.p = closest; 
        cameraPosition = ClosestPtPointPlane(cameraPosition, collisionPlane) +
                                          (collisionPlane.n * cameraCapsule.r) + 
                                          (collisionPlane.n * 0.002f);
    }
    else {
        obb->color = 0xFF00FF00;
    }
}




internal
OctreeNode *OctreeCreate(vec3 center, f32 halfWidth, i32 stopDepth, Arena *arena) {
    if(stopDepth < 0) {
        return NULL;
    }
    else {
        // construct and fill in 'root' of this subtree
        OctreeNode *node = ArenaPushStruct(arena, OctreeNode);
        node->center = center;
        node->halfWidth = halfWidth;
        node->objList = NULL;

        // Recursively construct the eight children of the subtree
        vec3 offset;
        f32 step = halfWidth * 0.5f;
        for(i32 i = 0; i < 8; ++i) {
            offset.x = ((i & 1) ? step : -step); 
            offset.y = ((i & 2) ? step : -step); 
            offset.z = ((i & 4) ? step : -step);
            node->child[i] = OctreeCreate(center + offset, step, stopDepth - 1, arena);
        }
        return node;
    }
}


internal
void OctreeInsertObject(OctreeNode *tree, StaticEntity *object, Arena *arena) {
    // if the OBB intersect with the octant add it to the octant object list
    bool intersect = false;
    for(i32 i = 0; i < object->meshCount; ++i) {
        OBB obb = {};
        obb.c = tree->center;
        obb.e = {tree->halfWidth, tree->halfWidth, tree->halfWidth};
        obb.u[0] = {1, 0, 0};
        obb.u[1] = {0, 1, 0};
        obb.u[2] = {0, 0, 1};
        if(TestOBBOBB(&object->obbs[i], &obb)) {
            intersect = true; 
        }
    }
    if(intersect) {
        // only insert into leaf nodes
        if(tree->child[0] == NULL) {
            // add the object to the link list
            if(tree->objList == NULL) {
                tree->objList = ArenaPushStruct(arena, StaticEntityNode);
                tree->objList->object = object;
                tree->objList->next = NULL;
            }
            else {
                StaticEntityNode *lastEntityNode = ArenaPushStruct(arena, StaticEntityNode);
                lastEntityNode->object = tree->objList->object;
                lastEntityNode->next = tree->objList->next;
                tree->objList->object = object;
                tree->objList->next = lastEntityNode;
            }
        }
        else {
            // if the node is not a leaf, recursively call inser in
            // all the childrens of the nodes
            for(i32 i = 0; i < 8; ++i) {
                OctreeInsertObject(tree->child[i], object, arena);
            }
        }
    }
}

void OctreeOBBQuery(OctreeNode *node, OBB *testOBB,
                    StaticEntityNode **entitiesList, i32 *entitiesCount,
                    Arena *outFrameArena) {
    OBB obb = {};
    obb.c = node->center;
    obb.e = {node->halfWidth, node->halfWidth, node->halfWidth};
    obb.u[0] = {1, 0, 0};
    obb.u[1] = {0, 1, 0};
    obb.u[2] = {0, 0, 1};
    if(TestOBBOBB(testOBB, &obb)) {
        if(node->child[0] ==  NULL) {
            // add childs of the node
            for(StaticEntityNode *entityNode = node->objList;
                entityNode != NULL;
                entityNode = entityNode->next) {
                // if the entity is already on the list, not put in again
                for(i32 i = 0; i < *entitiesCount; ++i) {
                    if(entityNode == ((*entitiesList) + i)) {
                        continue;
                    }
                }
                *entitiesList = ArenaPushStruct(outFrameArena, StaticEntityNode);
                *(*entitiesList) = *entityNode;
                *entitiesCount = *entitiesCount + 1;
            }
        }
        else {
            for(i32 i = 0; i < 8; ++i) {
                OctreeOBBQuery(node->child[i], testOBB, entitiesList, entitiesCount, outFrameArena);
            }
        }
    }
}

u32 colorArray[] = {
    0xFF0000FF,
    0xFF00FF00,
    0xFFFF0000,
    0xFF00FFFF,
    0xFFFFFF00,
    0xFFFF00FF,
    0xFF00F0F0
};

void DEBUG_DrawOctree(OctreeNode *tree, i32 colorIndex) {
    OBB obb;
    obb.c = tree->center;
    obb.e = {tree->halfWidth, tree->halfWidth, tree->halfWidth};
    mat4 translationMat = Mat4Translate(obb.c.x, obb.c.y, obb.c.z);
    mat4 scaleMat =  Mat4Scale(obb.e.x, obb.e.y, obb.e.z);
    obb.world = translationMat * scaleMat;
    obb.color = colorArray[colorIndex];
    DEBUG_RendererDrawWireframeBuffer(verticesCube, ARRAY_LENGTH(verticesCube), obb.color, obb.world);
    if(tree->child[0] != NULL) {
        for(i32 i = 0; i < 8; ++i) {
            DEBUG_DrawOctree(tree->child[i], (colorIndex + 1) % ARRAY_LENGTH(colorArray));
        }
    }
}


internal
void DrawAllObjectInsideOctree(OctreeNode *tree, GameState *gameState) {
    for(StaticEntityNode *entityNode = tree->objList;
        entityNode != NULL;
        entityNode = entityNode->next) {
        StaticEntity *staticEntity = entityNode->object;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            Mesh *mesh = staticEntity->meshes + j;
            OBB *obb = staticEntity->obbs + j;
            RendererPushWorkToQueue(mesh->vertices, mesh->indices, mesh->indicesCount,
                                    gameState->bitmap, {0.5f, 0.2f, -1}, mesh->world);
            DEBUG_RendererDrawWireframeBuffer(verticesCube, ARRAY_LENGTH(verticesCube), obb->color, obb->world);
        } 
    }
    if(tree->child[0] != NULL) {
        for(i32 i = 0; i < 8; ++i) {
            DrawAllObjectInsideOctree(tree->child[i], gameState);
        }
    }
}


internal
void DrawStaticEntityArray(StaticEntityNode *entities, i32 count, GameState *gameState) {
    for(i32 i = 0; i < count; ++i) {
        StaticEntityNode *entityNode = entities + i;
        StaticEntity *staticEntity = entityNode->object;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            Mesh *mesh = staticEntity->meshes + j;
            OBB *obb = staticEntity->obbs + j;
            RendererPushWorkToQueue(mesh->vertices, mesh->indices, mesh->indicesCount,
                                    gameState->bitmap, {0.5f, 0.2f, -1}, mesh->world);
            DEBUG_RendererDrawWireframeBuffer(verticesCube, ARRAY_LENGTH(verticesCube), obb->color, obb->world);
        }
    }
}

void GameInit(Memory *memory) {
    // The GameState has to be the first element on the memory
    ASSERT(memory->used + sizeof(GameState) <= memory->size);
    GameState *gameState = (GameState *)memory->data;
    memory->used += sizeof(GameState);

    WindowSystemInitialize(960, 540, "Last Hope 3D");
    RendererSystemInitialize();
    SoundSystemInitialize();
    
    gameState->dataArena = ArenaCreate(memory, Megabytes(500));
    gameState->frameArena = ArenaCreate(memory, Megabytes(10));
    gameState->textureArena = ArenaCreate(memory, Megabytes(1));
    gameState->soundArena = ArenaCreate(memory, Megabytes(1));
    gameState->staticEntitiesArena = ArenaCreate(memory, Megabytes(1));

    RendererSetProj(Mat4Perspective(60.0f, 960.0f/540.0f, 0.01f, 100.0f));
    RendererSetView(Mat4LookAt(cameraPosition, cameraPosition + cameraFront, cameraUp));

    // Load Assets
    gameState->bitmap = TextureCreate("../assets/test.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->chocolate = SoundCreate("../assets/chocolate.wav", &gameState->soundArena, &gameState->dataArena);
    gameState->music     = SoundCreate("../assets/lugia.wav", &gameState->soundArena, &gameState->dataArena);
    gameState->shoot     = SoundCreate("../assets/shoot.wav", &gameState->soundArena, &gameState->dataArena);

    // InitializeMap
    StaticEntitiesInitialized(&gameState->entities, &gameState->entitiesCount, &gameState->staticEntitiesArena,
                              vertices, indices, ARRAY_LENGTH(indices));


    
    // Init OBB
    vec3 cubeOBBRight = {1, 0, 0};
    vec3 cubeOBBUp    = {0, 1, 0};
    vec3 cubeOBBFront = {0, 0, 1};
    gameState->cubeOBB.c = {5, 0, 22};
    gameState->cubeOBB.u[0] = normalized(Mat3RotateY(RAD(0.0f)) * cubeOBBRight);
    gameState->cubeOBB.u[1] = normalized(Mat3RotateY(RAD(0.0f)) * cubeOBBUp);
    gameState->cubeOBB.u[2] = normalized(Mat3RotateY(RAD(0.0f)) * cubeOBBFront);
    gameState->cubeOBB.e = {2, 1, 1};
    mat4 translationMat = Mat4Translate(gameState->cubeOBB.c.x, gameState->cubeOBB.c.y, gameState->cubeOBB.c.z);
    mat4 rotationMat = Mat4Identity();//Mat4RotateX(RAD(0.0f)) * Mat4RotateY(RAD(0.0f)) * Mat4RotateZ(RAD(0.0f));
    mat4 scaleMat =  Mat4Scale(gameState->cubeOBB.e.x, gameState->cubeOBB.e.y, gameState->cubeOBB.e.z);
    gameState->cubeOBB.world = translationMat * rotationMat * scaleMat;
    gameState->cubeOBB.color = 0xFF00FF00;

    // TODO: test octree
    f32 mapWidth = mapCountX*2.0f;
    f32 mapHeight = mapCountY*2.0f; 
    gameState->tree = OctreeCreate({mapWidth*0.5f, -2.0f, mapHeight*0.5f},
                                    mapWidth*0.5f,
                                    1,
                                    &gameState->dataArena);

    // add the StaticEntity to the octree
    for(i32 i = 0; i < gameState->entitiesCount; ++i) {
        StaticEntity *object = gameState->entities + i;
        OctreeInsertObject(gameState->tree, object, &gameState->dataArena);
    }

    i32 StopHere = 0;

    cameraCapsule.a = cameraPosition;
    cameraCapsule.a.y += 0.2f;
    cameraCapsule.b = cameraPosition;
    cameraCapsule.b.y -= 0.6f;
    cameraCapsule.r = 0.3f;
    //SoundPlay(gameState->music, true);
}

void GameUpdate(Memory *memory, f32 dt) {
    GameState *gameState = (GameState *)memory->data;


    if(JoysickGetButtonJustDown(JOYSTICK_BUTTON_A)) {
        SoundPlay(gameState->shoot, false);
    }

    UpdateCamera(dt, gameState);
    
    OBB obb;
    obb.c = cameraPosition;
    obb.u[0] = {1, 0, 0};
    obb.u[1] = {0, 1, 0};
    obb.u[2] = {0, 0, 1};
    obb.e = {1, 1, 1};
    StaticEntityNode *entitiesToRender = NULL;
    i32 entitiesToRenderCount = 0;  
    OctreeOBBQuery(gameState->tree, &obb, &entitiesToRender, &entitiesToRenderCount, &gameState->frameArena);
    entitiesToRender = entitiesToRender - (entitiesToRenderCount - 1);

#if 0
    TestCameraCapsuleOBBsArray(entitiesToRender, entitiesToRenderCount);
    TestCameraCapsuleOBB(&gameState->cubeOBB);
#else
    TestCameraSphereOBBsArray(entitiesToRender, entitiesToRenderCount);
    TestCameraSphereOBB(&gameState->cubeOBB);
#endif

    RendererSetView(Mat4LookAt(cameraPosition, cameraPosition + cameraFront, cameraUp));
    cameraLastPosition = cameraPosition;

}

void GameRender(Memory *memory) {
    GameState *gameState = (GameState *)memory->data;
    RendererClearBuffers(0xFF021102, 0.0f);

    OBB obb;
    obb.c = cameraPosition;
    obb.u[0] = {1, 0, 0};
    obb.u[1] = {0, 1, 0};
    obb.u[2] = {0, 0, 1};
    obb.e = {1, 1, 1};
    StaticEntityNode *entitiesToRender = NULL;
    i32 entitiesToRenderCount = 0;  
    OctreeOBBQuery(gameState->tree, &obb, &entitiesToRender, &entitiesToRenderCount, &gameState->frameArena);
    entitiesToRender = entitiesToRender - (entitiesToRenderCount - 1);

    DrawStaticEntityArray(entitiesToRender, entitiesToRenderCount, gameState);
   

    //DrawAllObjectInsideOctree(gameState->tree, gameState);
    //DEBUG_DrawOctree(gameState->tree, 0);
    DEBUG_RendererDrawWireframeBuffer(verticesCube, ARRAY_LENGTH(verticesCube), gameState->cubeOBB.color, gameState->cubeOBB.world);
    
    RendererPresent();

    ArenaReset(&gameState->frameArena);
}

void GameShutdown(Memory * memory) {
    GameState *gameState = (GameState *)memory->data;
    SoundDestroy(gameState->shoot);
    SoundDestroy(gameState->music);
    SoundDestroy(gameState->chocolate);

    SoundSystemShudown();
    RendererSystemShutdown();
    WindowSystemShutdown();
}
