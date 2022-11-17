#include "lh_physics.h"
#include "lh_static_entity.h"
#include "lh_memory.h"

global_variable PhysicWorld gPhysicWorld;

void PhysicSystemInitialize() {
    gPhysicWorld.objects.Initialize(); 
}

void PhysicSystemShutdown() {
    // TODO: ...
}

SlotmapKey PhysicAddObject(PhysicObject **outObject) {
    PhysicObject object = {};
    SlotmapKey key = gPhysicWorld.objects.Add(object);
    *outObject = gPhysicWorld.objects.GetPtr(key);
    return key;
}

void PhysicRemoveObject(SlotmapKey key) {
    gPhysicWorld.objects.Remove(key);
}

internal
void Integrate(PhysicObject *object, f32 dt) {
    object->velocity = object->velocity + object->acceleration * dt;
    object->potentialPosition =  object->position + object->velocity * dt;
    if(object->grounded) {
        f32 damping = powf(0.001f, dt);
        object->velocity.x = object->velocity.x * damping;
        object->velocity.y = object->velocity.y * damping;
        object->velocity.z = object->velocity.z * damping;
    }
    else {
        f32 damping = powf(0.999f, dt);
        object->velocity.x = object->velocity.x * damping;
        object->velocity.y = object->velocity.y * damping;
        object->velocity.z = object->velocity.z * damping;    
    }
}

internal
void ProcessCollision(PhysicObject *object, OctreeNode *tree, Arena *arena, f32 dt) {

    OBB obb;
    obb.c = object->position;
    obb.u[0] = {1, 0, 0};
    obb.u[1] = {0, 1, 0};
    obb.u[2] = {0, 0, 1};
    obb.e = {1, 1, 1};
    StaticEntityNode *entitiesToProcess = NULL;
    i32 entitiesToProcessCount = 0;  
    OctreeOBBQuery(tree, &obb, &entitiesToProcess, &entitiesToProcessCount, arena);
    entitiesToProcess = entitiesToProcess - (entitiesToProcessCount - 1);

    // ray floor test
    bool flag = false;
    for(i32 i = 0; i < entitiesToProcessCount; ++i) {
        StaticEntityNode *entityNode = entitiesToProcess + i;
        StaticEntity *staticEntity = entityNode->object;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            OBB *obb = staticEntity->obbs + j;
            f32 t = 0;
            if(RaycastOBB(obb, &object->down, &t) && t <= 1.0f) {
                flag = true;
                if(object->velocity.y < 0) { 
                    object->velocity.y = 0;
                    object->acceleration.y = 0;
                }
            }
        }
    }
    object->grounded = flag;

    for(i32 i = 0; i < entitiesToProcessCount; ++i) {
        StaticEntityNode *entityNode = entitiesToProcess + i;
        StaticEntity *staticEntity = entityNode->object;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            OBB *obb = staticEntity->obbs + j;
            vec3 closest = ClosestPtPointOBB(object->position, obb);
            vec3 capsulePosition = ClosestPtPointSegment(closest, object->collider.a, object->collider.b);
            vec3 offset = capsulePosition - object->position;
            vec3 potentialCapsulePosition = object->potentialPosition + offset;
            
            Sphere sphere = {};
            sphere.c = capsulePosition;
            sphere.r = object->collider.r;
            vec3 d = potentialCapsulePosition - capsulePosition;
            f32 t = 0.0f;
            if(IntersectMovingSphereOBB(sphere, d, *obb, &t)) {
                vec3 hitPoint = capsulePosition + d * t;
                vec3 closestPoint = ClosestPtPointOBB(hitPoint, obb);
                vec3 normal = normalized(hitPoint - closestPoint);
                object->potentialPosition = (hitPoint + (normal * 0.002f) + (object->potentialPosition - potentialCapsulePosition));
                object->velocity = object->velocity - project(object->velocity, normal);
                vec3 scaleVelocity = object->velocity * (1.0f - t);
                object->potentialPosition = object->potentialPosition + scaleVelocity * dt;
            }
        }
    }
}

internal 
void UpdateCollisionData(PhysicObject *object, vec3 position) {
    object->collider.a = position;
    object->collider.a.y += 0.2f;
    object->collider.b = position;
    object->collider.b.y -= 0.8f;
    object->down.o = object->collider.b;
}

void PhysicStep(OctreeNode *tree, Arena *arena, f32 dt) {
    for(i32 i = 0; i < gPhysicWorld.objects.count; ++i) {
        PhysicObject *object = gPhysicWorld.objects.data + i;

        Integrate(object, dt);
        ProcessCollision(object, tree, arena, dt);
        object->position = object->potentialPosition;
        UpdateCollisionData(object, object->position);
    }
}

vec3 PhysicInterpolatePosition(PhysicObject *a, PhysicObject *b, f32 t) {
    vec3 result = a->position * (1.0f - t) + b->position * t;
    return result;
}

void PhysicAddForce(SlotmapKey key, vec3 force) {
    PhysicObject *object = gPhysicWorld.objects.GetPtr(key);
    object->acceleration = object->acceleration + force;
}

void PhysicAddImpulse(SlotmapKey key, vec3 impulse) {
    PhysicObject *object = gPhysicWorld.objects.GetPtr(key);
    object->velocity = object->velocity + impulse;
}
