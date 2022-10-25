#include "lh_game.h"
#include "lh_platform.h"
#include "lh_renderer.h"
#include "lh_sound.h"
#include "lh_texture.h"
#include "lh_input.h"

//////////////////////////////////////////////////////////////////////
// TODO (manuto):
//////////////////////////////////////////////////////////////////////
// - Try collision with the Scene
// - platform independent Debug output 
// - ...
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
vec3 cameraFront = {0, 0, 1};
vec3 cameraRight = {1, 0, 0};
vec3 cameraUp = {0, 1, 0};
f32 cameraPitch = 0;
f32 cameraYaw = RAD(90.0f);
bool cameraIsColliding = false;

f32 playerSpeed = 2.0f;
f32 sensitivity = 2.0f;

internal
void UpdateCamera(f32 dt) {
    f32 leftStickX = JoysickGetLeftStickX();
    f32 leftStickY = JoysickGetLeftStickY();
    f32 rightStickX = JoysickGetRightStickX();
    f32 rightStickY = JoysickGetRightStickY();
    
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


    // Left Stick movement
    cameraPosition = cameraPosition + (cameraRight * (leftStickX * playerSpeed)) * dt;
    cameraPosition = cameraPosition + (cameraFront * (leftStickY * playerSpeed)) * dt;    
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
void DrawStaticEntityArray(StaticEntity *entities,i32 count, GameState *gameState) {
    for(i32 i = 0; i < count; ++i) {
        StaticEntity *staticEntity = entities + i;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            Mesh *mesh = staticEntity->meshes + j;
            OBB *obb = staticEntity->obbs + j;
            RendererPushWorkToQueue(mesh->vertices, mesh->indices, mesh->indicesCount,
                                    gameState->bitmap, {0.5f, 0.2f, -1}, mesh->world);
            DEBUG_RendererDrawWireframeBuffer(verticesCube, ARRAY_LENGTH(verticesCube), obb->color, obb->world);
            
            mat4 world = Mat4Translate(obb->closestPoint.x, obb->closestPoint.y, obb->closestPoint.z) * Mat4Scale(0.02f, 0.02f, 0.02f);
            DEBUG_RendererDrawWireframeBuffer(verticesCube, ARRAY_LENGTH(verticesCube), 0xFF0000FF, world);
        }
    }
}

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
    rb =  b->e.v[1] * AbsR.m[0 * 3 + 2] + a->e.v[2] * AbsR.m[0 * 3 + 1];
    if(fabsf(t.v[2] *    R.m[1 * 3 + 0] -    t.v[1] *    R.m[2 * 3 + 0]) > ra + rb) return 0;

    // test axis L = AO x B1
    ra =  a->e.v[1] * AbsR.m[2 * 3 + 1] + a->e.v[2] * AbsR.m[1 * 3 + 1];
    rb =  b->e.v[0] * AbsR.m[0 * 3 + 2] + a->e.v[2] * AbsR.m[0 * 3 + 0];
    if(fabsf(t.v[2] *    R.m[1 * 3 + 1] -    t.v[1] *    R.m[2 * 3 + 1]) > ra + rb) return 0;

    // test axis L = AO x B2
    ra =  a->e.v[1] * AbsR.m[2 * 3 + 2] + a->e.v[2] * AbsR.m[1 * 3 + 2];
    rb =  b->e.v[0] * AbsR.m[0 * 3 + 1] + a->e.v[1] * AbsR.m[0 * 3 + 0];
    if(fabsf(t.v[2] *    R.m[1 * 3 + 2] -    t.v[1] *    R.m[2 * 3 + 2]) > ra + rb) return 0;

    // test axis L = A1 x B0
    ra =  a->e.v[0] * AbsR.m[2 * 3 + 0] + a->e.v[2] * AbsR.m[0 * 3 + 0];
    rb =  b->e.v[1] * AbsR.m[1 * 3 + 2] + a->e.v[2] * AbsR.m[1 * 3 + 1];
    if(fabsf(t.v[0] *    R.m[2 * 3 + 0] -    t.v[2] *    R.m[0 * 3 + 0]) > ra + rb) return 0;

    // test axis L = A1 x B1
    ra =  a->e.v[0] * AbsR.m[2 * 3 + 1] + a->e.v[2] * AbsR.m[0 * 3 + 1];
    rb =  b->e.v[0] * AbsR.m[1 * 3 + 2] + a->e.v[2] * AbsR.m[1 * 3 + 0];
    if(fabsf(t.v[0] *    R.m[2 * 3 + 1] -    t.v[2] *    R.m[0 * 3 + 1]) > ra + rb) return 0;

    // test axis L = A1 x B2
    ra =  a->e.v[0] * AbsR.m[2 * 3 + 2] + a->e.v[2] * AbsR.m[0 * 3 + 2];
    rb =  b->e.v[0] * AbsR.m[1 * 3 + 1] + a->e.v[1] * AbsR.m[1 * 3 + 0];
    if(fabsf(t.v[0] *    R.m[2 * 3 + 2] -    t.v[2] *    R.m[0 * 3 + 2]) > ra + rb) return 0;

    // test axis L = A2 x B0
    ra =  a->e.v[0] * AbsR.m[1 * 3 + 0] + a->e.v[1] * AbsR.m[0 * 3 + 0];
    rb =  b->e.v[1] * AbsR.m[2 * 3 + 2] + a->e.v[2] * AbsR.m[2 * 3 + 1];
    if(fabsf(t.v[1] *    R.m[0 * 3 + 0] -    t.v[0] *    R.m[1 * 3 + 0]) > ra + rb) return 0;

    // test axis l = A2 X B1
    ra =  a->e.v[0] * AbsR.m[1 * 3 + 1] + a->e.v[1] * AbsR.m[0 * 3 + 1];
    rb =  b->e.v[0] * AbsR.m[2 * 3 + 2] + a->e.v[2] * AbsR.m[2 * 3 + 0];
    if(fabsf(t.v[1] *    R.m[0 * 3 + 1] -    t.v[0] *    R.m[1 * 3 + 1]) > ra + rb) return 0;

    // test axis L = A2 x B1
    ra =  a->e.v[0] * AbsR.m[1 * 3 + 2] + a->e.v[1] * AbsR.m[0 * 3 + 2];
    rb =  b->e.v[0] * AbsR.m[2 * 3 + 1] + a->e.v[1] * AbsR.m[2 * 3 + 0];
    if(fabsf(t.v[1] *    R.m[0 * 3 + 2] -    t.v[0] *    R.m[1 * 3 + 2]) > ra + rb) return 0;
    
    return 1;
} 

// TODO: implement this better
const f32 NORMAL_EPSILON = 0.00001f;
#if 0
vec3 GetOBBNormalFromPoint(vec3 p, OBB *b) {
    vec3 pRel = p - b->c;
    f32 xDot = len(project(pRel, b->u[0]));
    if(fabsf(xDot - b->e.x) <= NORMAL_EPSILON) {
        return b->u[0];
    }
    else if(fabs(xDot - -b->e.x) <= NORMAL_EPSILON) {
        return b->u[0] * -1.0f;
    }
    f32 zDot = len(project(pRel, b->u[2]));
    if(fabs(zDot - b->e.z) <= NORMAL_EPSILON) {
        return b->u[2];
    }
    else if(fabs(zDot - -b->e.z) <= NORMAL_EPSILON) {
        return b->u[2] * -1.0f;
    }
    f32 yDot = len(project(pRel, b->u[1]));
    if(fabs(yDot - b->e.y) <= NORMAL_EPSILON) {
        return b->u[1];
    }
    else if(fabs(yDot - -b->e.y) <= NORMAL_EPSILON) {
        return b->u[1] * -1.0f;
    }
    ASSERT(!"FAILED");
    vec3 zero = {};
    return zero;
}
#else
vec3 GetOBBNormalFromPoint(vec3 p, OBB *b) {
    if(p.x == b->c.x + b->e.x) {
        return b->u[0];
    }
    if(p.x == b->c.x - b->e.x) {
        return b->u[0];
    }
    if(p.y == b->c.y + b->e.y) {
        return b->u[1];
    }
    if(p.y == b->c.y - b->e.y) {
        return b->u[1];
    }
    if(p.z == b->c.z + b->e.z) {
        return b->u[2];
    }
    if(p.z == b->c.z - b->e.z) {
        return b->u[2];
    }
}
#endif

void CameraCollisionResolutionOBB(OBB *obb) {
        vec3 collisionNormal = GetOBBNormalFromPoint(obb->closestPoint, obb);
        Plane collisionPlane;
        collisionPlane.n = normalized(collisionNormal);
        collisionPlane.p = obb->closestPoint;
        vec3 closest = ClosestPtPointPlane(cameraPosition, collisionPlane);
        cameraPosition.x = closest.x;
        cameraPosition.y = closest.y;
        cameraPosition.z = closest.z;
}

#include <windows.h>
#include <stdio.h>

void SortOBBArray(OBB *obbs, i32 count) {
    for(i32 j = 1;
        j < count;
        ++j)
    {
        OBB key = obbs[j];
        f32 keyDistance = lenSq(key.c - cameraPosition);
        i32 i = j - 1;
        
        OBB src = obbs[i];
        f32 srcDistance = lenSq(src.c - cameraPosition);
        while(i >= 0 && srcDistance > keyDistance)
        {
            obbs[i + 1] = obbs[i];
            --i;
            if(i >= 0) {
                srcDistance = lenSq(obbs[i].c - cameraPosition);;
            }
        }
        obbs[i + 1] = key;
    }
}

void CameraOBBsArray(StaticEntity *entities,i32 count) {
    for(i32 i = 0; i < count; ++i) {
        StaticEntity *staticEntity = entities + i;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            OBB *obb = staticEntity->obbs + j;
            
            f32 sqDistToClosestPoint = SqDistPointOBB(cameraPosition, obb);
            if(sqDistToClosestPoint > 0) {
                obb->closestPoint = ClosestPtPointOBB(cameraPosition, obb);
                obb->color = 0xFF00FF00;
            }
            if(sqDistToClosestPoint <= 0) {
                obb->color = 0xFFFF0000;
                CameraCollisionResolutionOBB(obb);
            }
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
    gameState->cubeOBB.u[0] = Mat3RotateY(RAD(45.0f)) * cubeOBBRight;
    gameState->cubeOBB.u[1] = Mat3RotateY(RAD(45.0f)) * cubeOBBUp;
    gameState->cubeOBB.u[2] = Mat3RotateY(RAD(45.0f)) * cubeOBBFront;
    gameState->cubeOBB.e = {2, 1, 1};
    mat4 translationMat = Mat4Translate(gameState->cubeOBB.c.x, gameState->cubeOBB.c.y, gameState->cubeOBB.c.z);
    mat4 rotationMat = Mat4RotateX(RAD(0.0f)) * Mat4RotateY(RAD(45.0f)) * Mat4RotateZ(RAD(0.0f));
    mat4 scaleMat =  Mat4Scale(gameState->cubeOBB.e.x, gameState->cubeOBB.e.y, gameState->cubeOBB.e.z);
    gameState->cubeOBB.world = translationMat * rotationMat  *scaleMat;
    gameState->cubeOBB.color = 0xFF00FF00;


    //SoundPlay(gameState->music, true);
}

void GameUpdate(Memory *memory, f32 dt) {
    GameState *gameState = (GameState *)memory->data;


    if(JoysickGetButtonJustDown(JOYSTICK_BUTTON_A)) {
        SoundPlay(gameState->shoot, false);
    }

    //CameraCollisionDetectionAndResolutionOBB(&gameState->cubeOBB);
    UpdateCamera(dt);
    CameraOBBsArray(gameState->entities, gameState->entitiesCount);

    RendererSetView(Mat4LookAt(cameraPosition, cameraPosition + cameraFront, cameraUp));

}

void GameRender(Memory *memory) {
    GameState *gameState = (GameState *)memory->data;
    RendererClearBuffers(0xFF021102, 0.0f);

    DrawStaticEntityArray(gameState->entities, gameState->entitiesCount, gameState);
    
    //DEBUG_RendererDrawWireframeBuffer(verticesCube, ARRAY_LENGTH(verticesCube), gameState->cubeOBB.color, gameState->cubeOBB.world);
    
    RendererPresent();
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
