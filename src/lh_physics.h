#ifndef _LH_PHYSICS_H_
#define _LH_PHYSICS_H_

#include "lh_defines.h"
#include "lh_math.h"
#include "lh_slotmap.h"
#include "lh_collision.h"

#define MAX_PHYSIC_OBJECTS_IN_WORLD 32


struct Arena;
struct OctreeNode;

struct PhysicObject {
    vec3 position;
    vec3 potentialPosition;
    vec3 velocity;
    vec3 acceleration;
    bool grounded;
    Capsule collider;
    Ray down;
};

struct PhysicWorld {
    Slotmap<PhysicObject, MAX_PHYSIC_OBJECTS_IN_WORLD> objects;
};

void PhysicSystemInitialize();
void PhysicSystemShutdown();
SlotmapKey PhysicAddObject(PhysicObject **outObject);
void PhysicRemoveObject(SlotmapKey key);
void PhysicStep(OctreeNode *tree, Arena *arena, f32 dt);
vec3 PhysicInterpolatePosition(PhysicObject *a, PhysicObject *b, f32 t);
void PhysicAddForce(SlotmapKey key, vec3 force);
void PhysicAddImpulse(SlotmapKey key, vec3 impulse);

#endif
