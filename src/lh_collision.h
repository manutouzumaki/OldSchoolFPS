#ifndef _LH_COLLISION_H_
#define _LH_COLLISION_H_

#include "lh_defines.h"
#include "lh_math.h"

struct Ray {
    vec3 o;
    vec3 d;
};

struct OBB {
    vec3 c;    // center point
    vec3 u[3]; // local x, y, and z axes
    vec3 e;    // positive halfwidth extents of OBB along each axis

    // Debug only
    mat4 world;
    u32 color;
};

struct Plane {
    vec3 n;  
    vec3 p;
};

struct Sphere {
    vec3 c;
    f32 r;
};

struct Capsule {
    vec3 a; // Medial line segment start point
    vec3 b; // Medial line segment end point
    f32  r; // Radius
};

struct Segment {
    vec3 a;
    vec3 b;
};


OBB CreateOBB(vec3 position, vec3 rotation, vec3 scale);
vec3 ClosestPtPointOBB(vec3 p, OBB *b);
f32 SqDistPointOBB(vec3 p, OBB *b);
i32 TestOBBOBB(OBB *a, OBB *b);
vec3 ClosestPtPointPlane(vec3 q, Plane plane);
f32 SqDistPointSegment(vec3 a, vec3 b, vec3 c);
vec3 ClosestPtPointSegment(vec3 c, vec3 a, vec3 b);
f32 ClosestPtSegmentSegment(vec3 p1, vec3 q1, vec3 p2, vec3 q2, f32 &s, f32 &t, vec3 &c1, vec3 &c2);
bool IntersectSegmentCapsule(Segment seg, vec3 a, vec3 b, f32 radii, f32 *tOut);
bool RaycastOBB(OBB *obb, Ray *ray, f32 *tOut);

#endif
