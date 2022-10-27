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
struct Mesh;



// TODO: start implementing simple collision detection and resolution
struct OBB {
    vec3 c;    // center point
    vec3 u[3]; // local x, y, and z axes
    vec3 e;    // positive halfwidth extents of OBB along each axis
    vec3 closestPoint;

    // Debug only
    mat4 world;
    u32 color;
};

struct Plane {
    vec3 n;  
    vec3 p;
};

struct StaticEntity {
    Transform transform;
    Mesh meshes[STATIC_ENTITY_MAX_MESH_COUNT];
    OBB obbs[STATIC_ENTITY_MAX_MESH_COUNT];
    i32 meshCount;
};

struct GameState {
    Arena dataArena;
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
};

void GameInit(Memory *memory);
void GameUpdate(Memory *memory, f32 dt);
void GameRender(Memory *memory);
void GameShutdown(Memory *memory);

#endif
