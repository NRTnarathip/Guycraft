#include <Chunk.h>
#include "ChunkMeshBuilding.h"
#include "ResourceManager.h"

Chunk::Chunk(glm::ivec2 pos) {
    this->pos = pos;
}
void Chunk::render() {
    auto resource = ResourceManager::GetInstance();
    auto defaultShader = resource->m_shaders["chunk_block_solid"];
    auto mcatlas = resource->m_textures["assets/textures/blocks/mcatlas.png"];
    defaultShader->Bind();
    glm::vec3 position = { pos.x, 0.f, pos.y };
    for (int i = 0; i < 8; i++) {
        auto mesh = &meshs[i];
        if (not mesh->isComplete) {
            continue;
        }

        // Mesh must be on gpu to draw
        glm::mat4 model = glm::mat4(1.f);
        model = glm::translate(model, position);
        defaultShader->SetMat4("model", model);
        mcatlas->Activate(GL_TEXTURE0);
        mesh->draw();
        position.y += CHUNK_SIZE;
    }
}
void Chunk::onLoad() {
    isLoad = true;
}
void Chunk::unload() {
    isLoad = false;
    unlinkChunkNeighbhor();
}
void Chunk::linkChunkNeighbor(Chunk* chunks[4]) {

}
void Chunk::unlinkChunkNeighbhor() {

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

void Chunk::generateMeshChunk() {
    isNeedGenerateMesh = false;

    Voxel voxTemp;
    bool isVoxelOnEdgeZ = false, isVoxelOnEdgeY = false;
    for (int group = 0; group < 8; group++) {
        auto mesh = &meshs[group];
        mesh->isComplete = false;
        for (u8 z = 0; z < CHUNK_SIZE; z++) {
            isVoxelOnEdgeZ = (z == 0 or z == CHUNK_SIZE_INDEX);
            for (u8 y = 0; y < CHUNK_SIZE; y++) {
                isVoxelOnEdgeY = (y == 0 or y == CHUNK_SIZE_INDEX);
                for (u8 x = 0; x < CHUNK_SIZE; x++) {
                    if (not isLoad) return; 

                    voxTemp = voxels[x + (y << 5) + (z << 10) + (group * CHUNK_SIZE_BLOCK)];
                    if (voxTemp.type == 0) continue; //dont gen block air
                    genMeshCube(mesh,group, x, y, z, voxTemp, x == 0 or x == CHUNK_SIZE_INDEX
                        or isVoxelOnEdgeY or isVoxelOnEdgeZ);
                }
            }
        }
        //end one task for gen mesh
        auto cMeshBuilding = ChunkMeshBuilding::GetInstance();
        cMeshBuilding->m_queueComplete.lock();
        cMeshBuilding->m_queueComplete.push(mesh);
        cMeshBuilding->m_queueComplete.unlock();
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

    return cNow->voxels[x + (y << 5) + (z << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type;
}
void Chunk::GetVoxSurround(unsigned char (&args)[27],u8 groupVoxel, unsigned char x, unsigned char y, unsigned char z,bool useFuncGetVoxOutChunk)
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
                args[index++] = voxels[i + (j << 5) + (k << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0?  0 : 1;
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
void Chunk::MakeQuadFace(MeshChunk* mesh,Voxel vox, unsigned char directFace, unsigned char (&voxSurr)[27], unsigned char (&lightSurr)[27]) {
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
        vert.SetUVIndex(i);
        vert.SetAO(aos[i]);
    }
}
void Chunk::genMeshCube(MeshChunk* mesh,u8 groupVoxel, char x, char y, char z, Voxel vox, bool useFuncGetVoxOutChunk) {
    char xx = x + 1;
    char yy = y + 1;
    char zz = z + 1;
    unsigned char voxSurr[27];
    unsigned char lightSurr[27];//lighting surround
    MeshChunk::Vertex vert;
    
    GetVoxSurround(voxSurr, groupVoxel, x, y, z, useFuncGetVoxOutChunk);
    vert.SetUVTile(vox.id, vox.type);
    if (FaceIsVisableUp(groupVoxel, x, yy, z))
    {
        vert.SetNormal(0);
        vert.SetPos(x, yy, z);
        mesh->vertexs.push_back(vert);//0
        vert.SetPos(x, yy, zz);
        mesh->vertexs.push_back(vert);//1
        vert.SetPos(xx, yy, zz);
        mesh->vertexs.push_back(vert);//2
        vert.SetPos(xx, yy, z);
        mesh->vertexs.push_back(vert);//3
        MakeQuadFace(mesh, vox, 0, voxSurr, lightSurr);
    }
    if (FaceIsVisableDown(groupVoxel, x,y - 1, z))
    {
        printf("voxel group %d and y %d\n", groupVoxel, y - 1);
        vert.SetNormal(1);
        vert.SetPos(x, y, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, y, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, y, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, y, zz);
        mesh->vertexs.push_back(vert);
        MakeQuadFace(mesh, vox, 1, voxSurr, lightSurr);
    }
    if (FaceIsVisableNorth(groupVoxel, x, y, zz))
    {
        vert.SetNormal(2);
        vert.SetPos(xx, y, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, yy, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, yy, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, y, zz);
        mesh->vertexs.push_back(vert);
        MakeQuadFace(mesh, vox, 2, voxSurr, lightSurr);
    }
    if (FaceIsVisableSouth(groupVoxel, x, y, z - 1))
    {
        vert.SetNormal(3);
        vert.SetPos(x, y, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, yy, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, yy, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, y, z);
        mesh->vertexs.push_back(vert);
        MakeQuadFace(mesh, vox, 3, voxSurr, lightSurr);
    }
    if (FaceIsVisableEast(groupVoxel, xx, y, z))
    {
        vert.SetNormal(4);
        vert.SetPos(xx, y, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, yy, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, yy, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(xx, y, zz);
        mesh->vertexs.push_back(vert);
        MakeQuadFace(mesh, vox, 4, voxSurr, lightSurr);
    }
    if (FaceIsVisableWest(groupVoxel, x - 1, y, z))
    {
        vert.SetNormal(5);
        vert.SetPos(x, y, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, yy, zz);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, yy, z);
        mesh->vertexs.push_back(vert);

        vert.SetPos(x, y, z);
        mesh->vertexs.push_back(vert);
        MakeQuadFace(mesh, vox, 5, voxSurr, lightSurr);
    }
}
//for check next block if type is air or transparency we should gen now mesh
bool Chunk::FaceIsVisableUp(u8 groupVoxel, char i, char j, char k)
{
    if (j > CHUNK_SIZE_INDEX)
    {
        //render if block is limit height of chunk
        if (groupVoxel == 7) {
            return true;
        }
        return voxels[i + (k << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0;
    }
    return voxels[i + (j << 5) + (k << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0;
}
bool Chunk::FaceIsVisableDown(u8 groupVoxel, char i, char j, char k)
{
    if (j < 0)
    {
        //render face if chunk is hiehgt is zero
        if (groupVoxel == 0) {
            printf("block out world \n");
            return true;
        }
        return voxels[i + (CHUNK_SIZE_INDEX << 5) + (k << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0;
    }
    return voxels[i + (j << 5) + (k << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0;
}
bool Chunk::FaceIsVisableNorth(u8 groupVoxel, char i, char j, char k)
{
    if (k > CHUNK_SIZE_INDEX)
    {
        if (north == nullptr) return false;
        return north->voxels[i + (j << 5) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0;
    }
    return voxels[i + (j << 5) + (k << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0;
}
bool Chunk::FaceIsVisableSouth(u8 groupVoxel, char i, char j, char k)
{
    if (k < 0)
    {
        if (south == nullptr) return false;
        return south->voxels[i + (j << 5) + (CHUNK_SIZE_INDEX << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0;
    }
    return voxels[i + (j << 5) + (k << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0;
}
bool Chunk::FaceIsVisableEast(u8 groupVoxel, char i, char j, char k)
{
    if (i > CHUNK_SIZE_INDEX)
    {
        if (east == nullptr) return false;
        return east->voxels[(j << 5) + (k << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0;
    }
    return voxels[i + (j << 5) + (k << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0;
}
bool Chunk::FaceIsVisableWest(u8 groupVoxel, char i, char j, char k)
{
    if (i < 0)
    {
        if (west == nullptr) return false;
        return west->voxels[CHUNK_SIZE_INDEX + (j << 5) + (k << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0;
    }
    return voxels[i + (j << 5) + (k << 10) + (groupVoxel * CHUNK_SIZE_BLOCK)].type == 0;
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
void Chunk::GetLightingSurround(unsigned char (&refLightMap)[27],int _x, int _y, int _z)
{
    u8 i = 0;
    for (int y = (int)(_y - 1); y <= _y + 1; y++)
    {
        for (int z = (int)(_z - 1); z <= _z + 1; z++)
        {
            for (int x = (int)(_x - 1); x <= _x + 1; x++)
            {
                u8 lightLevel = GetLightExtra(x, y, z);
                refLightMap[i++] = lightLevel;
            }
        }
    }
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