// TODO: map test

global_variable const i32 mapCountX = 16;
global_variable const i32 mapCountY = 16;
global_variable i32 map[mapCountY][mapCountX] = {
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
void StaticEntitiesInitialized(StaticEntity **entities, i32 *entitiesCount, Arena *arena,
                               Vertex *vertices, u32 *indices, i32 indicesCount, Texture **bitmaps,
                               Mesh *gpuMesh) {
    for(i32 y = 0; y < mapCountY; ++y) {
        for(i32 x = 0; x < mapCountX; ++x) {
            i32 tile = map[y][x];
            mat4 world = Mat4Identity();
            f32 xPos = x*2;
            f32 yPos = y*2;
            f32 offset = 1.0f;
            f32 tileOffset = 0.09f;
            switch(tile) {
                case 0: {
                    continue; 
                } break;
                case 1: {
                    *entities = ArenaPushStruct(arena, StaticEntity);
                    *entitiesCount = *entitiesCount + 1;
                    StaticEntity *entity = *entities;
                    entity->bitmap = bitmaps[1];
                    Transform *absTransform = &entity->transform;
                    Mesh *mesh = &entity->meshes[entity->meshCount];
                    OBB *obb = &entity->obbs[entity->meshCount++];
                    Transform *relTransform = &mesh->transform;
                    mat4 *world = &mesh->world;
                    
                    absTransform->position = {xPos, 0, yPos};
                    absTransform->scale = {1, 1, 1};
                    absTransform->rotation = {0, 0, 0};

                    relTransform->position = {0, -offset, 0};
                    relTransform->scale = {0, 0, 0};
                    relTransform->rotation = {90, 0, 0};

                    *obb = CreateOBB({xPos, -offset - tileOffset, yPos}, {0, 0, 0}, {1.0f, 0.1f, 1.0f});
                    *world = TransformToMat4(absTransform->position + relTransform->position,
                                             absTransform->rotation + relTransform->rotation,
                                             absTransform->scale + relTransform->scale);

                    mesh->vertices = vertices;
                    mesh->verticesCount = 0;
                    mesh->indices = indices;
                    mesh->indicesCount = indicesCount;
                    mesh->gpuVertex = gpuMesh->gpuVertex;
                    mesh->gpuIndices = gpuMesh->gpuIndices;

                } break;
                case 2: {
                    *entities = ArenaPushStruct(arena, StaticEntity);
                    *entitiesCount = *entitiesCount + 1;
                    StaticEntity *entity = *entities;
                    entity->bitmap = bitmaps[0];
                    Transform *absTransform = &entity->transform;
                    Mesh *mesh = &entity->meshes[entity->meshCount];
                    OBB *obb = &entity->obbs[entity->meshCount++];
                    Transform *relTransform = &mesh->transform;
                    mat4 *world = &mesh->world;
                    
                    absTransform->position = {xPos, 0, yPos};
                    absTransform->scale = {1, 1, 1};
                    absTransform->rotation = {0, 0, 0};

                    relTransform->position = {offset, 0, 0};
                    relTransform->scale = {0, 0, 0};
                    relTransform->rotation = {0, -90, 0};

                    *obb = CreateOBB({xPos + offset - tileOffset, 0, yPos}, {0, 0, 0}, {0.1f, 1.0f, 1.0f});
                    *world = TransformToMat4(absTransform->position + relTransform->position,
                                             absTransform->rotation + relTransform->rotation,
                                             absTransform->scale + relTransform->scale);
                    mesh->vertices = vertices;
                    mesh->verticesCount = 0;
                    mesh->indices = indices;
                    mesh->indicesCount = indicesCount;
                    mesh->gpuVertex = gpuMesh->gpuVertex;
                    mesh->gpuIndices = gpuMesh->gpuIndices;
                } break;
                case 4: {
                    *entities = ArenaPushStruct(arena, StaticEntity);
                    *entitiesCount = *entitiesCount + 1;
                    StaticEntity *entity = *entities;
                    entity->bitmap = bitmaps[0];
                    Transform *absTransform = &entity->transform;
                    Mesh *mesh = &entity->meshes[entity->meshCount];
                    OBB *obb = &entity->obbs[entity->meshCount++];
                    Transform *relTransform = &mesh->transform;
                    mat4 *world = &mesh->world;
                    
                    absTransform->position = {xPos, 0, yPos};
                    absTransform->scale = {1, 1, 1};
                    absTransform->rotation = {0, 0, 0};

                    relTransform->position = {-offset, 0, 0};
                    relTransform->scale = {0, 0, 0};
                    relTransform->rotation = {0, 90, 0};

                    *obb = CreateOBB({xPos - offset + tileOffset, 0, yPos}, {0, 0, 0}, {0.1f, 1.0f, 1.0f});
                    *world = TransformToMat4(absTransform->position + relTransform->position,
                                             absTransform->rotation + relTransform->rotation,
                                             absTransform->scale + relTransform->scale);
                    mesh->vertices = vertices;
                    mesh->verticesCount = 0;
                    mesh->indices = indices;
                    mesh->indicesCount = indicesCount;
                    mesh->gpuVertex = gpuMesh->gpuVertex;
                    mesh->gpuIndices = gpuMesh->gpuIndices;

                } break;
                case 5: {
                    *entities = ArenaPushStruct(arena, StaticEntity);
                    *entitiesCount = *entitiesCount + 1;
                    StaticEntity *entity = *entities;
                    entity->bitmap = bitmaps[0];
                    Transform *absTransform = &entity->transform;
                    Mesh *mesh = &entity->meshes[entity->meshCount];
                    OBB *obb = &entity->obbs[entity->meshCount++];
                    Transform *relTransform = &mesh->transform;
                    mat4 *world = &mesh->world;
                    
                    absTransform->position = {xPos, 0, yPos};
                    absTransform->scale = {1, 1, 1};
                    absTransform->rotation = {0, 0, 0};

                    relTransform->position = {0, 0, -offset};
                    relTransform->scale = {0, 0, 0};
                    relTransform->rotation = {0, 0, 0};

                    *obb = CreateOBB({xPos, 0, yPos - offset + tileOffset}, {0, 0, 0}, {1.0f, 1.0f, 0.1f});
                    *world = TransformToMat4(absTransform->position + relTransform->position,
                                             absTransform->rotation + relTransform->rotation,
                                             absTransform->scale + relTransform->scale);
                    mesh->vertices = vertices;
                    mesh->verticesCount = 0;
                    mesh->indices = indices;
                    mesh->indicesCount = indicesCount;
                    mesh->gpuVertex = gpuMesh->gpuVertex;
                    mesh->gpuIndices = gpuMesh->gpuIndices;

                } break;
                case 3: {
                    *entities = ArenaPushStruct(arena, StaticEntity);
                    *entitiesCount = *entitiesCount + 1;
                    StaticEntity *entity = *entities;
                    entity->bitmap = bitmaps[0];
                    Transform *absTransform = &entity->transform;
                    Mesh *mesh = &entity->meshes[entity->meshCount];
                    OBB *obb = &entity->obbs[entity->meshCount++];
                    Transform *relTransform = &mesh->transform;
                    mat4 *world = &mesh->world;
                    
                    absTransform->position = {xPos, 0, yPos};
                    absTransform->scale = {1, 1, 1};
                    absTransform->rotation = {0, 0, 0};

                    relTransform->position = {0, 0, offset};
                    relTransform->scale = {0, 0, 0};
                    relTransform->rotation = {0, 180, 0};

                    *obb = CreateOBB({xPos, 0, yPos + offset - tileOffset}, {0, 0, 0}, {1.0f, 1.0f, 0.1f});
                    *world = TransformToMat4(absTransform->position + relTransform->position,
                                             absTransform->rotation + relTransform->rotation,
                                             absTransform->scale + relTransform->scale);

                    mesh->vertices = vertices;
                    mesh->verticesCount = 0;
                    mesh->indices = indices;
                    mesh->indicesCount = indicesCount;
                    mesh->gpuVertex = gpuMesh->gpuVertex;
                    mesh->gpuIndices = gpuMesh->gpuIndices;

                } break;
                case 6: {
                    *entities = ArenaPushStruct(arena, StaticEntity);
                    *entitiesCount = *entitiesCount + 1;
                    StaticEntity *entity = *entities;
                    entity->bitmap = bitmaps[0];
                    Transform *absTransform = &entity->transform;
                    Mesh *mesh1 = &entity->meshes[entity->meshCount];
                    OBB *obb1 = &entity->obbs[entity->meshCount++];
                    Transform *relTransform1 = &mesh1->transform;
                    mat4 *world1 = &mesh1->world;
                    Mesh *mesh2 = &entity->meshes[entity->meshCount];
                    OBB *obb2 = &entity->obbs[entity->meshCount++];
                    Transform *relTransform2 = &mesh2->transform;
                    mat4 *world2 = &mesh2->world;
                    
                    absTransform->position = {xPos, 0, yPos};
                    absTransform->scale = {1, 1, 1};
                    absTransform->rotation = {0, 0, 0};

                    relTransform1->position = {-offset, 0, 0};
                    relTransform1->scale = {0, 0, 0};
                    relTransform1->rotation = {0, 90, 0};

                    *obb1 = CreateOBB({xPos - offset + tileOffset, 0, yPos}, {0, 0, 0}, {0.1f, 1.0f, 1.0f});
                    *world1 = TransformToMat4(absTransform->position + relTransform1->position,
                                              absTransform->rotation + relTransform1->rotation,
                                              absTransform->scale + relTransform1->scale);

                    relTransform2->position = {0, 0, offset};
                    relTransform2->scale = {0, 0, 0};
                    relTransform2->rotation = {0, 180, 0};

                    *obb2 = CreateOBB({xPos, 0, yPos + offset - tileOffset}, {0, 0, 0}, {1.0f, 1.0f, 0.1f});
                    *world2 = TransformToMat4(absTransform->position + relTransform2->position,
                                              absTransform->rotation + relTransform2->rotation,
                                              absTransform->scale + relTransform2->scale);
                    mesh1->vertices = vertices;
                    mesh1->verticesCount = 0;
                    mesh1->indices = indices;
                    mesh1->indicesCount = indicesCount;
                    mesh1->gpuVertex = gpuMesh->gpuVertex;
                    mesh1->gpuIndices = gpuMesh->gpuIndices;

                    mesh2->vertices = vertices;
                    mesh2->verticesCount = 0;
                    mesh2->indices = indices;
                    mesh2->indicesCount = indicesCount;
                    mesh2->gpuVertex = gpuMesh->gpuVertex;
                    mesh2->gpuIndices = gpuMesh->gpuIndices;
                } break;
                case 7: {
                    *entities = ArenaPushStruct(arena, StaticEntity);
                    *entitiesCount = *entitiesCount + 1;
                    StaticEntity *entity = *entities;
                    entity->bitmap = bitmaps[0];
                    Transform *absTransform = &entity->transform;
                    Mesh *mesh1 = &entity->meshes[entity->meshCount];
                    OBB *obb1 = &entity->obbs[entity->meshCount++];
                    Transform *relTransform1 = &mesh1->transform;
                    mat4 *world1 = &mesh1->world;
                    Mesh *mesh2 = &entity->meshes[entity->meshCount];
                    OBB *obb2 = &entity->obbs[entity->meshCount++];
                    Transform *relTransform2 = &mesh2->transform;
                    mat4 *world2 = &mesh2->world;
                    
                    absTransform->position = {xPos, 0, yPos};
                    absTransform->scale = {1, 1, 1};
                    absTransform->rotation = {0, 0, 0};

                    relTransform1->position = {offset, 0, 0};
                    relTransform1->scale = {0, 0, 0};
                    relTransform1->rotation = {0, -90, 0};

                    *obb1 = CreateOBB({xPos + offset - tileOffset, 0, yPos}, {0, 0, 0}, {0.1f, 1.0f, 1.0f});
                    *world1 = TransformToMat4(absTransform->position + relTransform1->position,
                                              absTransform->rotation + relTransform1->rotation,
                                              absTransform->scale + relTransform1->scale);

                    relTransform2->position = {0, 0, offset};
                    relTransform2->scale = {0, 0, 0};
                    relTransform2->rotation = {0, 180, 0};

                    *obb2 = CreateOBB({xPos, 0, yPos + offset - tileOffset}, {0, 0, 0}, {1.0f, 1.0f, 0.1f});
                    *world2 = TransformToMat4(absTransform->position + relTransform2->position,
                                              absTransform->rotation + relTransform2->rotation,
                                              absTransform->scale + relTransform2->scale);
                    mesh1->vertices = vertices;
                    mesh1->verticesCount = 0;
                    mesh1->indices = indices;
                    mesh1->indicesCount = indicesCount;
                    mesh1->gpuVertex = gpuMesh->gpuVertex;
                    mesh1->gpuIndices = gpuMesh->gpuIndices;

                    mesh2->vertices = vertices;
                    mesh2->verticesCount = 0;
                    mesh2->indices = indices;
                    mesh2->indicesCount = indicesCount;
                    mesh2->gpuVertex = gpuMesh->gpuVertex;
                    mesh2->gpuIndices = gpuMesh->gpuIndices;

                } break;
                case 8: {
                    *entities = ArenaPushStruct(arena, StaticEntity);
                    *entitiesCount = *entitiesCount + 1;
                    StaticEntity *entity = *entities;
                    entity->bitmap = bitmaps[0];
                    Transform *absTransform = &entity->transform;
                    Mesh *mesh1 = &entity->meshes[entity->meshCount];
                    OBB *obb1 = &entity->obbs[entity->meshCount++];
                    Transform *relTransform1 = &mesh1->transform;
                    mat4 *world1 = &mesh1->world;
                    Mesh *mesh2 = &entity->meshes[entity->meshCount];
                    OBB *obb2 = &entity->obbs[entity->meshCount++];
                    Transform *relTransform2 = &mesh2->transform;
                    mat4 *world2 = &mesh2->world;
                    
                    absTransform->position = {xPos, 0, yPos};
                    absTransform->scale = {1, 1, 1};
                    absTransform->rotation = {0, 0, 0};

                    relTransform1->position = {0, 0, -offset};
                    relTransform1->scale = {0, 0, 0};
                    relTransform1->rotation = {0, 0, 0};

                    *obb1 = CreateOBB({xPos, 0, yPos - offset + tileOffset}, {0, 0, 0}, {1.0f, 1.0f, 0.1f});
                    *world1 = TransformToMat4(absTransform->position + relTransform1->position,
                                              absTransform->rotation + relTransform1->rotation,
                                              absTransform->scale + relTransform1->scale);

                    relTransform2->position = {-offset, 0, 0};
                    relTransform2->scale = {0, 0, 0};
                    relTransform2->rotation = {0, 90, 0};

                    *obb2 = CreateOBB({xPos - offset + tileOffset, 0, yPos}, {0, 0, 0}, {0.1f, 1.0f, 1.0f});
                    *world2 = TransformToMat4(absTransform->position + relTransform2->position,
                                              absTransform->rotation + relTransform2->rotation,
                                              absTransform->scale + relTransform2->scale);
                    mesh1->vertices = vertices;
                    mesh1->verticesCount = 0;
                    mesh1->indices = indices;
                    mesh1->indicesCount = indicesCount;
                    mesh1->gpuVertex = gpuMesh->gpuVertex;
                    mesh1->gpuIndices = gpuMesh->gpuIndices;
                    
                    mesh2->vertices = vertices;
                    mesh2->verticesCount = 0;
                    mesh2->indices = indices;
                    mesh2->indicesCount = indicesCount;
                    mesh2->gpuVertex = gpuMesh->gpuVertex;
                    mesh2->gpuIndices = gpuMesh->gpuIndices;
                } break;
                case 9: {
                    *entities = ArenaPushStruct(arena, StaticEntity);
                    *entitiesCount = *entitiesCount + 1;
                    StaticEntity *entity = *entities;
                    entity->bitmap = bitmaps[0];
                    Transform *absTransform = &entity->transform;
                    Mesh *mesh1 = &entity->meshes[entity->meshCount];
                    OBB *obb1 = &entity->obbs[entity->meshCount++];
                    Transform *relTransform1 = &mesh1->transform;
                    mat4 *world1 = &mesh1->world;
                    Mesh *mesh2 = &entity->meshes[entity->meshCount];
                    OBB *obb2 = &entity->obbs[entity->meshCount++];
                    Transform *relTransform2 = &mesh2->transform;
                    mat4 *world2 = &mesh2->world;
                    
                    absTransform->position = {xPos, 0, yPos};
                    absTransform->scale = {1, 1, 1};
                    absTransform->rotation = {0, 0, 0};

                    relTransform1->position = {offset, 0, 0};
                    relTransform1->scale = {0, 0, 0};
                    relTransform1->rotation = {0, -90, 0};

                    *obb1 = CreateOBB({xPos + offset - tileOffset, 0, yPos}, {0, 0, 0}, {0.1f, 1.0f, 1.0f});
                    *world1 = TransformToMat4(absTransform->position + relTransform1->position,
                                              absTransform->rotation + relTransform1->rotation,
                                              absTransform->scale + relTransform1->scale);

                    relTransform2->position = {0, 0, -offset};
                    relTransform2->scale = {0, 0, 0};
                    relTransform2->rotation = {0, 0, 0};

                    *obb2 = CreateOBB({xPos, 0, yPos - offset + tileOffset}, {0, 0, 0}, {1.0f, 1.0f, 0.1f});
                    *world2 = TransformToMat4(absTransform->position + relTransform2->position,
                                              absTransform->rotation + relTransform2->rotation,
                                              absTransform->scale + relTransform2->scale);
                    mesh1->vertices = vertices;
                    mesh1->verticesCount = 0;
                    mesh1->indices = indices;
                    mesh1->indicesCount = indicesCount;
                    mesh1->gpuVertex = gpuMesh->gpuVertex;
                    mesh1->gpuIndices = gpuMesh->gpuIndices;
                    
                    mesh2->vertices = vertices;
                    mesh2->verticesCount = 0;
                    mesh2->indices = indices;
                    mesh2->indicesCount = indicesCount;
                    mesh2->gpuVertex = gpuMesh->gpuVertex;
                    mesh2->gpuIndices = gpuMesh->gpuIndices;
                } break;
            }; 

        }
    }
    // set the pointer to the bigining of the array
    *entities = *entities - (*entitiesCount - 1);
}
