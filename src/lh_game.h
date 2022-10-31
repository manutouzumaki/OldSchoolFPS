#ifndef _LH_GAME_H_
#define _LH_GAME_H_

#include "lh_defines.h"
#include "lh_memory.h"
#include "lh_math.h"

// TODO: try to remove this from herer
#include "lh_renderer.h"

#define STATIC_ENTITY_MAX_MESH_COUNT 6

struct Sound;
struct Texture;

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

struct Capsule {
    vec3 a; // Medial line segment start point
    vec3 b; // Medial line segment end point
    f32  r; // Radius
};

struct StaticEntity {
    Transform transform;
    Mesh meshes[STATIC_ENTITY_MAX_MESH_COUNT];
    OBB obbs[STATIC_ENTITY_MAX_MESH_COUNT];
    i32 meshCount;
};

struct StaticEntityNode {
    StaticEntity *object;
    StaticEntityNode *next;
};


// octree node data structure
struct OctreeNode {
    vec3 center;                // center point of the octree
    f32 halfWidth;              // half the width of the node volumen
    OctreeNode *child[8];       // pointers to the eight children nodes
    StaticEntityNode *objList;  // linked list of the object in this node
};

struct GameState {
    Arena dataArena;
    Arena frameArena;
    Arena textureArena; 
    Arena soundArena;
    Arena staticEntitiesArena;
    
    Texture *bitmap;
    Sound *chocolate;
    Sound *music;    
    Sound *shoot;

    StaticEntity *entities;
    i32 entitiesCount;

    OBB cubeOBB;

    i32 mouseDefaultScreenX;
    i32 mouseDefaultScreenY;

    OctreeNode *tree;
};

void GameInit(Memory *memory);
void GameUpdate(Memory *memory, f32 dt);
void GameRender(Memory *memory);
void GameShutdown(Memory *memory);

#endif
