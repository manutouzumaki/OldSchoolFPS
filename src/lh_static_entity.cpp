#include "lh_static_entity.h"
#include "lh_memory.h"

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

void OctreeInsertLight(OctreeNode *tree, Light *object, Arena *arena) {
    // if the OBB intersect with the octant add it to the octant object list
    OBB obb = {};
    obb.c = tree->center;
    obb.e = {tree->halfWidth, tree->halfWidth, tree->halfWidth};
    obb.u[0] = {1, 0, 0};
    obb.u[1] = {0, 1, 0};
    obb.u[2] = {0, 0, 1};
    if(TestPointOBB(&obb, object->position)) {
        // only insert into leaf nodes
        if(tree->child[0] == NULL) {
            // add the object to the link list
            if(tree->lightList == NULL) {
                tree->lightList = ArenaPushStruct(arena, LightNode);
                tree->lightList->object = object;
                tree->lightList->next = NULL;
            }
            else {
                LightNode *lastLightNode = ArenaPushStruct(arena, LightNode);
                lastLightNode->object = tree->lightList->object;
                lastLightNode->next = tree->lightList->next;
                tree->lightList->object = object;
                tree->lightList->next = lastLightNode;
            }
        }
        else {
            // if the node is not a leaf, recursively call inser in
            // all the childrens of the nodes
            for(i32 i = 0; i < 8; ++i) {
                OctreeInsertLight(tree->child[i], object, arena);
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

void OctreeOBBQueryLights(OctreeNode *node, OBB *testOBB,
                    LightNode **lightsList, i32 *lightsCount,
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
            for(LightNode *lightNode = node->lightList;
                lightNode != NULL;
                lightNode = lightNode->next) { 
                // if the entity is already on the list, not put in again
                bool flag = false;
                for(i32 i = 0; i < *lightsCount; ++i) {
                    LightNode *lights = (*lightsList) - (*lightsCount - 1);
                    LightNode *tmpLightNode = lights + i;
                    if(lightNode->object->position.x == tmpLightNode->object->position.x &&
                       lightNode->object->position.y == tmpLightNode->object->position.y &&
                       lightNode->object->position.z == tmpLightNode->object->position.z) {
                        flag = true;
                        break;
                    }
                }
                if(flag) continue;
                *lightsList = ArenaPushStruct(outFrameArena, LightNode);
                *(*lightsList) = *lightNode;
                *lightsCount = *lightsCount + 1;
            
            }
        }
        else {
            for(i32 i = 0; i < 8; ++i) {
                OctreeOBBQueryLights(node->child[i], testOBB, lightsList, lightsCount, outFrameArena);
            }
        }
    }
}
