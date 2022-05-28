#include <Chunk.h>
#include "ChunkMeshBuilding.h"
#include "ResourceManager.h"
#include "ChunkManager.h"
#include "BlockSystem/BlockDatabase.h"
#include <algorithm>
#include <iostream>

#define ACCESSVOXEL voxels[x + (y << 4) + (z << 8) + (group << 12)]
#define IMP_RETURN_VOXELISSOLID u8 type = ACCESSVOXEL.type; return type !=0 and type != 4;

std::vector<Chunk*> Chunk::getAllChunkNeighbor()
{
    return  { north,south, east,west,northEast, northWest, southEast,southWest };
}
void Chunk::render() {
    auto resource = ResourceManager::GetInstance();
    auto sdSolid = resource->m_shaders["chunk_block_solid"];
    auto sdFluid = resource->m_shaders["chunk_block_fluid"];
    auto mcatlas = resource->m_textures["assets/textures/blocks/mcatlas.png"];
    glm::vec3 position = { pos.x, 0.f, pos.y };
    for (int i = 0; i < VOXELGROUP_COUNT; i++) {
        auto m = &meshs[i];
        m->lock();
        if (not m->isComplete) {
            m->unlock();
            continue;
        }
        glm::mat4 model = glm::mat4(1.f);
        // Mesh must be on gpu to draw
        position.y = i * CHUNK_SIZE;
        model = glm::translate(model, position);
        //render mesh solid
        sdSolid->Bind();
        sdSolid->SetMat4("model", model);
        mcatlas->Activate(GL_TEXTURE0);
        m->solid.draw();

        sdFluid->Bind();
        sdFluid->SetMat4("model", model);
        mcatlas->Activate(GL_TEXTURE0);
        m->fluid.draw();

        m->unlock();
    }
}
void Chunk::unload() {
    isLoad = false;
    for (auto& mesh : meshs) {
        mesh.lock();
        mesh.clear();
        mesh.unlock();
    }
    mutexNeighbor.lock();
    if (northEast != nullptr) {
        northEast->m_allocateChunkNeighbor--;
        northEast->southWest = nullptr;
        northEast = nullptr;
    }
    if (northWest != nullptr) {
        northWest->m_allocateChunkNeighbor--;
        northWest->southEast = nullptr;
        northWest = nullptr;
    }
    if (southEast != nullptr) {
        southEast->m_allocateChunkNeighbor--;
        southEast->northWest = nullptr;
        southEast = nullptr;
    }
    if (southWest != nullptr) {
        southWest->m_allocateChunkNeighbor--;
        southWest->northEast = nullptr;
        southWest = nullptr;
    }
    if (north != nullptr) {
        north->m_allocateChunkNeighbor--;
        north->south = nullptr;
        north = nullptr;
    }
    if (south != nullptr) {
        south->m_allocateChunkNeighbor--;
        south->north = nullptr;
        south = nullptr;
    }
    if (east != nullptr) {
        east->m_allocateChunkNeighbor--;
        east->west = nullptr;
        east = nullptr;
    }
    if (west != nullptr) {
        west->m_allocateChunkNeighbor--;
        west->east = nullptr;
        west = nullptr;
    }
    mutexNeighbor.unlock();
  
}
int Chunk::getHasChunkNeighborCount() {
    int count = 0;
    auto cnb = getAllChunkNeighbor();
    for (auto c : cnb)
        if (c != nullptr) count++;
    return count;
}
void Chunk::linkChunkNeighbor(Chunk* chunks[8]) {
    auto cNorth = chunks[0];
    auto cSouth = chunks[1];
    auto cEast = chunks[2];
    auto cWest = chunks[3];
    auto cNorthEast = chunks[4];
    auto cSouthEast = chunks[5];
    auto cSouthWest = chunks[6];
    auto cNorthWest = chunks[7];

    if (cNorthEast != nullptr) {
        cNorthEast->southWest = this;
        northEast = cNorthEast;
    }
    if (cSouthEast != nullptr) {
        cSouthEast->northWest = this;
        southEast = cSouthEast;
    }
    if (cSouthWest != nullptr) {
        cSouthWest->northEast = this;
        southWest = cSouthWest;
    }
    if (cNorthWest != nullptr) {
        cNorthWest->southEast = this;
        northWest = cNorthWest;
    }

    if (cNorth != nullptr) {
        north = cNorth;
        cNorth->south = this;
    }
    if (cSouth != nullptr) {
        south = cSouth;
        cSouth->north = this;
    }
    if (cEast != nullptr) {
        east = cEast;
        cEast->west = this;
    }
    if (cWest != nullptr) {
        west = cWest;
        cWest->east = this;
    }
}
u16 Chunk::getBlockCount() {
    u16 i = 0;
    for (auto voxel : voxels) if (voxel.type != 0) i++;
    return i;
}
bool Chunk::isEmpty() {
    for (auto voxel : voxels) if (voxel.type != 0) return false;
    return true;
}
Voxel Chunk::getvoxel(u8 group, u8 x, u8 y, u8 z) {
    if (y > CHUNK_SIZE_INDEX) {
        if (group == VOXELGROUP_INDEX) return { 0,0 };
        y = 0;
        group++;
    }
    else if (y < 0) {
        if (group == 0) return { 0, 0 };
        y = CHUNK_SIZE;
        group--;
    }
    return ACCESSVOXEL;
};
void Chunk::generateMesh(int voxelGroup) {
    auto mesh = &meshs[voxelGroup];
    mesh->lock();
    mesh->isNeedGenMesh = false;
    mesh->unlock();
    Voxel* voxel;
    bool isVoxelOnEdgeZ = false, isVoxelOnEdgeY = false;
    auto cMeshBuilding = ChunkMeshBuilding::GetInstance();

    for (u8 z = 0; z < CHUNK_SIZE; z++) {
        isVoxelOnEdgeZ = (z == 0 or z == CHUNK_SIZE_INDEX);
        for (u8 y = 0; y < CHUNK_SIZE; y++) {
            isVoxelOnEdgeY = (y == 0 or y == CHUNK_SIZE_INDEX);
            for (u8 x = 0; x < CHUNK_SIZE; x++) {
                mesh->lock();
                if (mesh->isNeedGenMesh or not isLoad) {
                    mesh->isNeedGenMesh = false;
                    mesh->unlock();
                    return;
                }
                mesh->unlock();

                voxel = &voxels[x + (y << 4) + (z << 8) + (voxelGroup << 12)];
                if (voxel->type == 0) continue; //dont gen block air

                bool useFuncGetVoxelOutChunk = x == 0 or x == CHUNK_SIZE_INDEX
                    or isVoxelOnEdgeY or isVoxelOnEdgeZ;

                mutexNeighbor.lock();
                if (voxel->type == 4) {
                    genMeshWater(&mesh->fluid, voxelGroup, x, y, z, voxel, useFuncGetVoxelOutChunk);
                }
                else {
                    genMeshCube(&mesh->solid, voxelGroup, x, y, z, voxel, useFuncGetVoxelOutChunk);
                }
                mutexNeighbor.unlock();
            }
        }
    }
    //end one task for gen mesh
    cMeshBuilding->m_queueComplete.pushLock(mesh);
}
void Chunk::genMeshWater(MeshChunk* mesh,u8 groupVoxel, char x, char y, char z, Voxel* voxel, bool useFuncGetVoxelOutChunk)
{
    MeshChunk::Vertex vert;
    vert.SetUVTile(4);//water
    u8 voxSurr[27];
    u8 lightSurr[27];
    GetVoxSurround(voxSurr, groupVoxel, x, y, z, useFuncGetVoxelOutChunk);
    i8 xx = x + 1;
    i8 yy = y + 1;
    i8 zz = z + 1;
    float waterLevel = 15;
    float vertexY = y + (waterLevel / 16);
    //gen mesh water top away
    if (getvoxel(groupVoxel,x,yy,z).type == 0) {
        vert.SetNormal(0);
        vert.SetPos(x, vertexY, z);
        mesh->vertexs.push_back(vert);//0
        vert.SetPos(x, vertexY, zz);
        mesh->vertexs.push_back(vert);//1
        vert.SetPos(xx, vertexY, zz);
        mesh->vertexs.push_back(vert);//2
        vert.SetPos(xx, vertexY, z);
        mesh->vertexs.push_back(vert);//3
        MakeQuadFace(mesh,  0, voxSurr, lightSurr);
    }
}
unsigned char Chunk::GetVoxType(u8 groupVoxel,char x, char y, char z)
{
    Chunk* cNow = this;
    if (x < 0)
    {
        if (west == nullptr) return 0;
        cNow = west;
        x = CHUNK_SIZE_INDEX;
    }
    else if (x > CHUNK_SIZE_INDEX)
    {
        if (east == nullptr) return 0;
        cNow = east;
        x = 0;
    }
    if (y < 0) {
        if (groupVoxel == 0) return 0;
        groupVoxel--;
        y = CHUNK_SIZE_INDEX;
    }
    else if (y > CHUNK_SIZE_INDEX) {
        if (groupVoxel == VOXELGROUP_COUNT - 1) return 0;
        groupVoxel++;
        y = 0;
    }
    if (z < 0)
    {
        if (cNow->south == nullptr) return 0;
        cNow = cNow->south;
        z = CHUNK_SIZE_INDEX;
    }
    else if (z > CHUNK_SIZE_INDEX)
    {
        if (cNow->north == nullptr) return 0;
        cNow = cNow->north;
        z = 0;
    }

    return cNow->voxels[x + (y << 4) + (z << 8) + (groupVoxel << 12)].type;
}
void Chunk::GetVoxSurround(unsigned char (&args)[27],u8 groupVoxel, u8 x, u8 y, u8 z,bool useFuncGetVoxOutChunk)
{
    u8 index = 0;
    //optimize
    //use function check next voxel of this chunk
    if (useFuncGetVoxOutChunk) {
        for (int j = y - 1; j <= y + 1; j++)
            for (int k = z - 1; k <= z + 1; k++)
                for (int i = x - 1; i <= x + 1; i++)
                    args[index++] = GetVoxType(groupVoxel, i,j,k) == 0 ? 0 : 1;
        return;
    }
    //get voxel in chunk
    for (u8 j = y - 1; j <= y + 1; j++)
        for (u8 k = z - 1; k <= z + 1; k++)
            for (u8 i = x - 1; i <= x + 1; i++)
                args[index++] = voxels[i + (j << 4) + (k << 8) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0?  0 : 1;
}
void Chunk::GetAO(unsigned char (&refAO)[4], unsigned char dir, unsigned char (&voxSurr)[27]) {
    if (dir == 0)//Face up
    {
        refAO[0] = CalculateAO(voxSurr[19], voxSurr[21], voxSurr[18]);
        refAO[1] = CalculateAO(voxSurr[21], voxSurr[25], voxSurr[24]);
        refAO[2] = CalculateAO(voxSurr[25], voxSurr[23], voxSurr[26]);
        refAO[3] = CalculateAO(voxSurr[23], voxSurr[19], voxSurr[20]);
        return;
    }
    else if (dir == 1)//down
    {
        refAO[0] = CalculateAO(voxSurr[7], voxSurr[3], voxSurr[6]);
        refAO[1] = CalculateAO(voxSurr[3], voxSurr[1], voxSurr[0]);
        refAO[2] = CalculateAO(voxSurr[1], voxSurr[5], voxSurr[2]);
        refAO[3] = CalculateAO(voxSurr[5], voxSurr[7], voxSurr[8]);
        return;
    }
    else if (dir == 2)//north
    {
        refAO[0] = CalculateAO(voxSurr[7], voxSurr[17], voxSurr[8]);
        refAO[1] = CalculateAO(voxSurr[17], voxSurr[25], voxSurr[26]);
        refAO[2] = CalculateAO(voxSurr[25], voxSurr[15], voxSurr[24]);
        refAO[3] = CalculateAO(voxSurr[15], voxSurr[7], voxSurr[6]);
        return;
    }
    else if (dir == 3)//south
    {
        refAO[0] = CalculateAO(voxSurr[1], voxSurr[9], voxSurr[0]);
        refAO[1] = CalculateAO(voxSurr[9], voxSurr[19], voxSurr[18]);
        refAO[2] = CalculateAO(voxSurr[19], voxSurr[11], voxSurr[20]);
        refAO[3] = CalculateAO(voxSurr[11], voxSurr[1], voxSurr[2]);
        return;
    }
    else if (dir == 4)//east
    {
        refAO[0] = CalculateAO(voxSurr[5], voxSurr[11], voxSurr[2]);
        refAO[1] = CalculateAO(voxSurr[11], voxSurr[23], voxSurr[20]);
        refAO[2] = CalculateAO(voxSurr[23], voxSurr[17], voxSurr[26]);
        refAO[3] = CalculateAO(voxSurr[17], voxSurr[5], voxSurr[8]);
        return;
    }
    //west
    refAO[0] = CalculateAO(voxSurr[3], voxSurr[15], voxSurr[6]);
    refAO[1] = CalculateAO(voxSurr[15], voxSurr[21], voxSurr[24]);
    refAO[2] = CalculateAO(voxSurr[21], voxSurr[9], voxSurr[18]);
    refAO[3] = CalculateAO(voxSurr[9], voxSurr[3], voxSurr[0]);
    return;
}
unsigned char Chunk::CalculateAO(unsigned char side1, unsigned char side2, unsigned char corner) {
    if (side1 == 1 && side2 == 1)
    {
        return 3;
    }
    return (unsigned char)(side1 + side2 + corner);
}
void Chunk::GetVertLightMaping(unsigned char(&ref)[4], unsigned char dir, unsigned char (&lightSurr)[27]) {
    if (dir == 0) {//up
        ref[0] = lightSurr[22];
        ref[1] = lightSurr[22];
        ref[2] = lightSurr[22];
        ref[3] = lightSurr[22];
    }
    else if (dir == 1) {//down
        ref[0] = lightSurr[4];
        ref[1] = lightSurr[4];
        ref[2] = lightSurr[4];
        ref[3] = lightSurr[4];
    }
    else if (dir == 2) {//north
        ref[0] = lightSurr[16];
        ref[1] = lightSurr[16];
        ref[2] = lightSurr[16];
        ref[3] = lightSurr[16];
    }
    else if (dir == 3) {//south
        ref[0] = lightSurr[10];
        ref[1] = lightSurr[10];
        ref[2] = lightSurr[10];
        ref[3] = lightSurr[10];
    }
    else if (dir == 4) {//east
        ref[0] = lightSurr[14];
        ref[1] = lightSurr[14];
        ref[2] = lightSurr[14];
        ref[3] = lightSurr[14];
    }
    //west
    ref[0] = lightSurr[12];
    ref[1] = lightSurr[12];
    ref[2] = lightSurr[12];
    ref[3] = lightSurr[12];
}
void Chunk::MakeQuadFace(MeshChunk* mesh, unsigned char directFace, unsigned char (&voxSurr)[27], unsigned char (&lightSurr)[27]) {
    u32 vertCount = mesh->vertexs.size();
    u32 vertCountMinus_1 = vertCount - 1;//index 3
    u32 vertCountMinus_2 = vertCount - 2;//index 2
    u32 vertCountMinus_3 = vertCount - 3;//index 1
    u32 vertCountMinus_4 = vertCount - 4;//index 0

    mesh->triangles.push_back(vertCountMinus_4);//0
    mesh->triangles.push_back(vertCountMinus_3);//1
    mesh->triangles.push_back(vertCountMinus_1);//3

    mesh->triangles.push_back(vertCountMinus_2);//2
    mesh->triangles.push_back(vertCountMinus_1);//3
    mesh->triangles.push_back(vertCountMinus_3);//1

    //ao use 2bit 0->3;
    //2bit *4 = 8bit or 1 byte;
    u8 aos[4];
    //light map between sun and lamp light
    u8 lightMaping[4];
    GetAO(aos, directFace, voxSurr);
    //flip ao if

    GetVertLightMaping(lightMaping, directFace, lightSurr);

    //lightmap
    //set vertex from vertCount - 4 to vertcount - 1;
    for (u8 i = 0; i < 4; i++) {
        auto& vert = mesh->vertexs[vertCount - (4 - i)];
        vert.setVertexIndex(i);
        vert.SetAO(aos[i]);
    }
}
void Chunk::genMeshCube(MeshChunk* mesh,u8 groupVoxel, char x, char y, char z, Voxel* vox, bool useFuncGetVoxOutChunk) {
    char xx = x + 1;
    char yy = y + 1;
    char zz = z + 1;
    unsigned char voxSurr[27];
    unsigned char lightSurr[27];//lighting surround
    MeshChunk::Vertex vert;
    GetVoxSurround(voxSurr, groupVoxel, x, y, z, useFuncGetVoxOutChunk);
    auto blockDB = BlockDatabase::GetInstance();
    auto model = blockDB->m_models[vox->type];
    using FaceDir = BlockModel::FaceDirection;
    int uvTiles[6];
    int tile = vox->type;
    for (auto elem : model->m_textures) {
        if (elem.dir == FaceDir::All) {
            std::fill(std::begin(uvTiles),std::end(uvTiles), 
                blockDB->m_texturesTileIndex[elem.texture]);
        }
    }

    if (not voxelUpIsSolid(groupVoxel, x, yy, z))
    {
        vert.SetUVTile(uvTiles[0]);
        vert.SetNormal(0);
        vert.SetPos(x, yy, z);
        mesh->vertexs.push_back(vert);//0
        vert.SetPos(x, yy, zz);
        mesh->vertexs.push_back(vert);//1
        vert.SetPos(xx, yy, zz);
        mesh->vertexs.push_back(vert);//2
        vert.SetPos(xx, yy, z);
        mesh->vertexs.push_back(vert);//3
        MakeQuadFace(mesh, 0, voxSurr, lightSurr);
    }
    if (not voxelDownIsSolid(groupVoxel, x,y - 1, z))
    {
        vert.SetUVTile(uvTiles[1]);
        vert.SetNormal(1);
        vert.SetPos(x, y, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, y, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, y, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, y, zz);
        mesh->vertexs.push_back(vert);
        MakeQuadFace(mesh,  1, voxSurr, lightSurr);
    }
    if (not voxelNorthIsSolid(groupVoxel, x, y, zz))
    {
        vert.SetUVTile(uvTiles[2]);
        vert.SetNormal(2);
        vert.SetPos(xx, y, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, yy, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, yy, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, y, zz);
        mesh->vertexs.push_back(vert);
        MakeQuadFace(mesh,  2, voxSurr, lightSurr);
    }
    if (not voxelSouthIsSolid(groupVoxel, x, y, z - 1))
    {
        vert.SetUVTile(uvTiles[3]);
        vert.SetNormal(3);
        vert.SetPos(x, y, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, yy, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, yy, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, y, z);
        mesh->vertexs.push_back(vert);
        MakeQuadFace(mesh, 3, voxSurr, lightSurr);
    }
    if (not voxelEastIsSolid(groupVoxel, xx, y, z))
    {
        vert.SetUVTile(uvTiles[4]);
        vert.SetNormal(4);
        vert.SetPos(xx, y, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, yy, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, yy, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, y, zz);
        mesh->vertexs.push_back(vert);
        MakeQuadFace(mesh,  4, voxSurr, lightSurr);
    }
    if (not voxelWestIsSolid(groupVoxel, x - 1, y, z))
    {
        vert.SetUVTile(uvTiles[5]);
        vert.SetNormal(5);
        vert.SetPos(x, y, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, yy, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, yy, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, y, z);
        mesh->vertexs.push_back(vert);
        MakeQuadFace(mesh,  5, voxSurr, lightSurr);
    }
}
unsigned char Chunk::GetLightExtra(int x, int y, int z)
{/*
    Chunk* cNow = this;
    if (x < 0)
    {
        if (cnearWest == nullptr) return 0;
        cNow = cnearWest;
        x = 15;
    }
    else if (x > 15)
    {
        if (cnearEast == nullptr) return 0;
        cNow = cnearEast;
        x = 0;
    }
    if (y < 0)
    {
        if (cNow->cnearDown == nullptr) return 0;
        cNow = cNow->cnearDown;
        y = 15;
    }
    else if (y > 15)
    {
        if (cNow->cnearUp == nullptr) return 0;
        cNow = cNow->cnearUp;
        y = 0;
    }
    if (z < 0)
    {
        if (cNow->cnearUp == nullptr) return 0;
        cNow = cNow->cnearUp;
        z = 15;
    }
    else if (z > 15)
    {
        if (cNow->cnearUp == nullptr) return 0;
        cNow = cNow->cnearUp;
        z = 0;
    }
    return cNow->lightMap[x + (y << 4) + (z << 8)];*/
    return 15;
}
unsigned char Chunk::GetSunLight(unsigned short acc) {
    return lightMap[acc] & 15;
}
unsigned char Chunk::GetLampLight(unsigned short acc) {
    return (lightMap[acc] >> 4 ) & 15;
}
void Chunk::SetLampLight(unsigned short acc, unsigned char val) {
    //mask 240;//bit 1111 0000
    lightMap[acc] = (lightMap[acc] & 0xF0 )| (val);
}
void Chunk::SetSunLight(unsigned short acc, unsigned char val) {
    //mask 15; bit 0000 1111
    lightMap[acc] = (lightMap[acc]& 0x0F) | (val << 4);
}

bool Chunk::voxelUpIsSolid(u8 group, i8 x, i8 y, i8 z) {
    if (y > CHUNK_SIZE_INDEX) {
        if (group == VOXELGROUP_INDEX) return false;
        u8 type = voxels[x + (z << 8) + ((group+1) << 12)].type;
        return type != 0 and type != 4;
    }
    IMP_RETURN_VOXELISSOLID
};
bool Chunk::voxelDownIsSolid(u8 group, i8 x, i8 y, i8 z) {
    if (y < 0) {
        if (group == 0) return false;
        u8 type = voxels[x + (15 << 4) + (z << 8) + ((group - 1) << 12)].type;
        return type != 0 and type != 4;
    }
    IMP_RETURN_VOXELISSOLID
}
bool Chunk::voxelNorthIsSolid(u8 group, i8 x, i8 y, i8 z) {
    if (z > CHUNK_SIZE_INDEX) {
        if (north == nullptr) return true;
        u8 type = north->voxels[x + (y << 4) + (group << 12)].type;
        return type != 0 and type != 4;
    }
    IMP_RETURN_VOXELISSOLID
}
bool Chunk::voxelSouthIsSolid(u8 group, i8 x, i8 y, i8 z) {
    if (z < 0) {
        if (south == nullptr) return true;
        u8 type = south->voxels[x + (y << 4) + (15 << 8) + (group << 12)].type;
        return type != 0 and type != 4;
    }
    IMP_RETURN_VOXELISSOLID
}
bool Chunk::voxelEastIsSolid(u8 group, i8 x, i8 y, i8 z) {
    if (x > CHUNK_SIZE_INDEX) {
        if (east == nullptr) return true;
        u8 type = east->voxels[(y << 4) + (z<<8) + (group << 12)].type;
        return type != 0 and type != 4;
    }
    IMP_RETURN_VOXELISSOLID
}
bool Chunk::voxelWestIsSolid(u8 group, i8 x, i8 y, i8 z) {
    if (x < 0) {
        if (west == nullptr) return true;
        u8 type = west->voxels[15 + (y << 4) + (z << 8) + (group << 12)].type;
        return type != 0 and type != 4;
    }
    IMP_RETURN_VOXELISSOLID
}