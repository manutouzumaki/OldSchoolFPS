#ifndef _LH_STATIC_ENTITY_H_
#define _LH_STATIC_ENTITY_H_

#include "lh_defines.h"
#include "lh_math.h"
// TODO: try to remove this from herer
#include "lh_renderer.h"
#include "lh_collision.h"

#define STATIC_ENTITY_MAX_MESH_COUNT 6

struct Arena;

struct StaticEntity {
    Transform transform;
    Mesh meshes[STATIC_ENTITY_MAX_MESH_COUNT];
    OBB obbs[STATIC_ENTITY_MAX_MESH_COUNT];
    Texture *bitmap;
    i32 meshCount;
};

struct Light {
    vec3 position;
    // TODO: add:
    // - color
    // - diffuseStrength
    // - specularStrength
    // - etc ...
};

struct StaticEntityNode {
    StaticEntity *object;
    StaticEntityNode *next;
};

struct LightNode {
    Light *object;
    LightNode *next;
};



// octree node data structure
struct OctreeNode {
    vec3 center;                // center point of the octree
    f32 halfWidth;              // half the width of the node volumen
    OctreeNode *child[8];       // pointers to the eight children nodes
    StaticEntityNode *objList;  // linked list of the object in this node
    LightNode *lightList;       // linked list of the lights in this node
};

OctreeNode *OctreeCreate(vec3 center, f32 halfWidth, i32 stopDepth, Arena *arena);
void OctreeInsertObject(OctreeNode *tree, StaticEntity *object, Arena *arena);
void OctreeInsertLight(OctreeNode *tree, Light *light, Arena *arena);
void OctreeOBBQuery(OctreeNode *node, OBB *testOBB, StaticEntityNode **entitiesList,
                    i32 *entitiesCount, Arena *outFrameArena);
void OctreeOBBQueryLights(OctreeNode *node, OBB *testOBB, LightNode **lightsList, i32 *lightsCount,
                    Arena *outFrameArena);

#endif
