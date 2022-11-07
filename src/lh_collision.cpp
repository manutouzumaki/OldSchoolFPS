#include "lh_collision.h"

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

vec3 ClosestPtPointPlane(vec3 q, Plane plane) {
    vec3 n = plane.n;
    vec3 p = plane.p;
    f32 t = dot(n, q - p) / dot(n , n);
    vec3 r = q - (n * t);
    return r;
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

internal
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
    if(a <= FLT_EPSILON  && e <= FLT_EPSILON) { 
        // Both segments degenerate into points
        s = t = 0.0f;
        c1 = p1;
        c2 = p2;
        return dot(c1 - c2, c1 - c2);
    }
    if(a <= FLT_EPSILON) {
        // first segment degenerate into a point
        s = 0.0f;
        t = f / e;
        t = Clamp(t, 0.0f, 1.0f); 
    }
    else {
        f32 c = dot(d1, r);
        if(e <= FLT_EPSILON) {
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

vec3 ClosestPtPointSphere(vec3 p, Sphere s) {
    vec3 sphereToPoint = p - s.c;
    normalize(&sphereToPoint);
    sphereToPoint = sphereToPoint * s.r;
    return sphereToPoint + s.c;
}


i32 IntersectSegmentSphere(Segment seg, Sphere s, f32 *t) {
    vec3 d = seg.b - seg.a;
    vec3 m = seg.a - s.c;
    f32 b = dot(m, d);
    f32 c = dot(m, m) - s.r * s.r;
    if(c > 0.0f && b > 0.0f) return 0;
    f32 discr = b*b - c;
    if(discr < 0.0f) return 0;
    *t = -b - sqrtf(discr);
    if(*t < 0.0f) *t = 0.0f;
    return 1;
}

bool IntersectSegmentCapsule(Segment seg, vec3 segA, vec3 segB, f32 radii, f32 *tOut) {
    vec3 c1 = {};
    vec3 c2 = {};
    f32 s = 0;
    f32 t = 0;
    f32 sqDist =  ClosestPtSegmentSegment(seg.a, seg.b, segA, segB, s, t, c1, c2); 
    bool result = sqDist <= radii*radii;
    if(result) {
        Sphere s;
        s.c = c2;
        s.r = radii;
        IntersectSegmentSphere(seg, s, tOut); 
        return true;
    }

    return false;
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
        *tOut = 0.0f;
        return true;
    }
    *tOut = tmin;
    return true;
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


i32 IntersectMovingSphereOBB(Sphere s, vec3 d, OBB b, f32 *t) {
    // Compute the OBB resulting from expanding b by sphere radius r
    OBB e = b;
    e.e.x += s.r;
    e.e.y += s.r;
    e.e.z += s.r;
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

    // ?Or? all set bits together into a bit mask (note: here u + v == u | v)
    i32 m = u + v;
    // Define line segment [c, c+d] specified by the sphere movement
    Segment seg = {s.c, s.c + d};
    // If all 3 bits set (m == 7) then p is in a vertex region
    if (m == 7) {
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
