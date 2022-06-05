#include "ChunkSection.h"
#include "ChunkMeshBuilding.h"
#include "BlockSystem/BlockDatabase.h"
#include "ResourceManager.h"
#include <glm/gtx/string_cast.hpp>

void ChunkSection::unLoad() {
    m_mesh->lock();
    m_mesh->stateGenerateMesh = StateGenerateMesh::Empty;
    m_mesh->unlock();
    m_mesh->clear();
}

void ChunkSection::setSunLight(uint8_t x, uint8_t y, uint8_t z)
{
}
uint8_t ChunkSection::getTourchLight(uint8_t x, uint8_t y, uint8_t z)
{
    return uint8_t();
}
void ChunkSection::changeBlocks(blocks_t blocks)
{
    for (int z = 0; z < CHUNK_SIZE; z++)
        for (int y = 0; y < CHUNK_SIZE; y++)
            for (int x = 0; x < CHUNK_SIZE; x++) {
                int acc = x + (y << 4) + (z << 8);
                auto block = blocks[acc];
                if (block.type != 0) {
                    m_isBlocksEmpty = false;
                }
                m_blocks[acc] = block;
            }
}
uint8_t ChunkSection::getSunLight(uint8_t x, uint8_t y, uint8_t z)
{
    return uint8_t();
}
void ChunkSection::setBlock(uint8_t x, uint8_t y, uint8_t z, Block block)
{
    if (block.type != 0) {
        m_isBlocksEmpty = false;
    }
    m_blocks[x + (y<<4) + (z<<8)] = block;
}
void ChunkSection::setBlock(int key, Block block)
{
    if (block.type != 0) {
        m_isBlocksEmpty = false;
    }
    m_blocks[key] = block;
}
void ChunkSection::generateMesh() {
    //clear mesh before gen
    m_mesh->fluid.clearDataOnGenerate();
    m_mesh->solid.clearDataOnGenerate();
    m_mesh->lock();
    m_mesh->stateGenerateMesh = StateGenerateMesh::OnGenerate;
    m_mesh->unlock();

    auto blockDB = BlockDatabase::GetInstance();
    auto &blockDBModels = blockDB->m_models;
    Block* block;
    bool isVoxelOnEdgeZ = false, isVoxelOnEdgeY = false;
    auto cMeshBuilding = ChunkMeshBuilding::GetInstance();
    for (u8 z = 0; z < CHUNK_SIZE; z++) {
        m_mesh->lock();
        if (m_mesh->stateGenerateMesh == StateGenerateMesh::Empty or
            m_mesh->stateGenerateMesh == StateGenerateMesh::OnNeedGenerate) {
            m_mesh->unlock();
            m_mesh->fluid.clearDataOnGenerate();
            m_mesh->solid.clearDataOnGenerate();
            return;
        }
        m_mesh->unlock();
        isVoxelOnEdgeZ = (z == 0 or z == CHUNK_SIZE_INDEX);
        for (u8 y = 0; y < CHUNK_SIZE; y++) {
            isVoxelOnEdgeY = (y == 0 or y == CHUNK_SIZE_INDEX);
            for (u8 x = 0; x < CHUNK_SIZE; x++) {
                block = &m_blocks[x + (y << 4) + (z << 8)];
                if (block->type == 0) {
                    continue; //dont gen block air
                }
                bool useFuncGetVoxelOutChunk = x == 0 or x == CHUNK_SIZE_INDEX
                    or isVoxelOnEdgeY or isVoxelOnEdgeZ;
                auto blockModel = blockDBModels[block->type];
                auto blockShape = blockModel->m_shape;
                if (blockShape == Shape::Cube) {
                    genMeshSolid(block, x, y, z,useFuncGetVoxelOutChunk);
                }
                else if(blockShape == Shape::Water) {
                    genMeshWater(block, x, y, z, useFuncGetVoxelOutChunk);
                }
            }
        }
    }
    m_mesh->lock();
    m_mesh->stateGenerateMesh = StateGenerateMesh::Empty;
    m_mesh->unlock();
    bool isNeedTransferToGPU = m_mesh->fluid.vertexs.size() > 0 or m_mesh->solid.vertexs.size() > 0;
    if (isNeedTransferToGPU) {
        cMeshBuilding->m_queueComplete.pushLock(m_mesh);
    }
}

void ChunkSection::genMeshSolid(Block* block, int8_t x, int8_t y, int8_t z,
    bool useFuncGetBlockOutChunk)
{
    char xx = x + 1;
    char yy = y + 1;
    char zz = z + 1;
    uint8_t voxSurr[27];
    uint8_t lightSurr[27];//lighting surround
    MeshChunk::Vertex vert;
    getLightSurround(lightSurr, x, y, z, useFuncGetBlockOutChunk);
    getBlockSurround(voxSurr, x, y, z, useFuncGetBlockOutChunk);
    glm::vec2 blockUVs[6];

    using FaceDir = BlockModel::FaceDirection;
    auto blockType = block->type;
    auto model = BlockDatabase::GetInstance()->m_models[blockType];
    auto atlas = ResourceManager::GetInstance()->m_textureAtlas["chunk"];
    for (auto blockTexture : model->m_textures) {
        auto dir = blockTexture.dir;
        std::string textureID = blockTexture.textureID;
        auto uv = atlas->m_textures[textureID].start;
        if (dir == FaceDir::All) {
            blockUVs[0] = uv;
            blockUVs[1] = uv;
            blockUVs[2] = uv;
            blockUVs[3] = uv;
            blockUVs[4] = uv;
            blockUVs[5] = uv;
        }
        else if (dir == FaceDir::Side) {
            blockUVs[2] = uv;
            blockUVs[3] = uv;
            blockUVs[4] = uv;
            blockUVs[5] = uv;
        }
        else if (dir == FaceDir::Top) {
            blockUVs[0] = uv;
        }
        else if (dir == FaceDir::Buttom) {
            blockUVs[1] = uv;
        }
    }
    auto mesh = &m_mesh->solid;
    auto& vertexs = mesh->vertexs.m_vector;

    if (not checkBlockIsShape(Shape::Cube, 0, x, y, z)) {
        vert.setUV(blockUVs[0]);
        vert.setNormal(0);
        vert.setPos(x, yy, z);
        vertexs.push_back(vert);//0
        vert.setPos(x, yy, zz);
        vertexs.push_back(vert);//1
        vert.setPos(xx, yy, zz);
        vertexs.push_back(vert);//2
        vert.setPos(xx, yy, z);
        vertexs.push_back(vert);//3
        makeFaceBlock(mesh, 0, voxSurr, lightSurr);
    }
    if (not checkBlockIsShape(Shape::Cube, 1, x, y, z))
    {
        vert.setUV(blockUVs[1]);
        vert.setNormal(1);
        vert.setPos(x, y, zz);
        vertexs.push_back(vert);

        vert.setPos(x, y, z);
        vertexs.push_back(vert);

        vert.setPos(xx, y, z);
        vertexs.push_back(vert);

        vert.setPos(xx, y, zz);
        vertexs.push_back(vert);
        makeFaceBlock(mesh, 1, voxSurr, lightSurr);
    }
    if (not checkBlockIsShape(Shape::Cube, 2, x, y, z))
    {
        vert.setUV(blockUVs[2]);
        vert.setNormal(2);
        vert.setPos(xx, y, zz);
        mesh->vertexs.push_back(vert);

        vert.setPos(xx, yy, zz);
        mesh->vertexs.push_back(vert);

        vert.setPos(x, yy, zz);
        mesh->vertexs.push_back(vert);

        vert.setPos(x, y, zz);
        mesh->vertexs.push_back(vert);
        makeFaceBlock(mesh, 2, voxSurr, lightSurr);

    }
    if (not checkBlockIsShape(Shape::Cube, 3, x, y, z))
    {
        vert.setUV(blockUVs[3]);
        vert.setNormal(3);
        vert.setPos(x, y, z);

        mesh->vertexs.push_back(vert);

        vert.setPos(x, yy, z);
        mesh->vertexs.push_back(vert);

        vert.setPos(xx, yy, z);
        mesh->vertexs.push_back(vert);

        vert.setPos(xx, y, z);
        mesh->vertexs.push_back(vert);
        makeFaceBlock(mesh, 3, voxSurr, lightSurr);

    }
    if (not checkBlockIsShape(Shape::Cube, 4, x, y, z))
    {
        vert.setUV(blockUVs[4]);
        vert.setNormal(4);
        vert.setPos(xx, y, z);
        mesh->vertexs.push_back(vert);

        vert.setPos(xx, yy, z);
        mesh->vertexs.push_back(vert);

        vert.setPos(xx, yy, zz);
        mesh->vertexs.push_back(vert);

        vert.setPos(xx, y, zz);
        mesh->vertexs.push_back(vert);
        makeFaceBlock(mesh, 4, voxSurr, lightSurr);
    }
    if (not checkBlockIsShape(Shape::Cube, 5, x, y, z))
    {
        vert.setUV(blockUVs[5]);
        vert.setNormal(5);
        vert.setPos(x, y, zz);
        mesh->vertexs.push_back(vert);

        vert.setPos(x, yy, zz);
        mesh->vertexs.push_back(vert);

        vert.setPos(x, yy, z);
        mesh->vertexs.push_back(vert);

        vert.setPos(x, y, z);
        mesh->vertexs.push_back(vert);
        makeFaceBlock(mesh, 5, voxSurr, lightSurr);
    }
}

void ChunkSection::genMeshWater(Block* block, int8_t x, int8_t y, int8_t z,
    bool useFuncGetBlockOutChunk)
{
    int xx = x + 1;
    int yy = y + 1;
    int zz = z + 1;
    float waterLevel = 15;
    float vertexY = y + (waterLevel / 16);
    unsigned char voxSurr[27];
    uint8_t lightSurr[27];//lighting surround
    MeshChunk::Vertex vert;
    getLightSurround(lightSurr, x, y, z, useFuncGetBlockOutChunk);
    getBlockSurround(voxSurr, x, y, z, useFuncGetBlockOutChunk);
    auto blockDB = BlockDatabase::GetInstance();
    auto model = blockDB->m_models[block->type];
    auto atlas = ResourceManager::GetInstance()->m_textureAtlas["chunk"];
    using FaceDir = BlockModel::FaceDirection;
    glm::vec2 uvTop;
    for (auto elem : model->m_textures) {
        if (elem.dir == FaceDir::All) {
            uvTop = atlas->m_textures[elem.textureID].start;
        }
    }
    auto mesh = &m_mesh->fluid;
    auto& vertexs = mesh->vertexs.m_vector;
    if (not checkBlockIsShape(Shape::Water, 0, x, y, z)) {
        vert.setUV(uvTop);
        vert.setNormal(0);
        vert.setPos(x, vertexY, z);
        vertexs.push_back(vert);//0
        vert.setPos(x, vertexY, zz);
        vertexs.push_back(vert);//1
        vert.setPos(xx, vertexY, zz);
        vertexs.push_back(vert);//2
        vert.setPos(xx, vertexY, z);
        vertexs.push_back(vert);//3
        makeFaceBlock(mesh, 0, voxSurr, lightSurr);
    }
}
void ChunkSection::makeFaceBlock(MeshChunk* mesh, uint8_t directFace, uint8_t(&voxSurr)[27], uint8_t(&lightSurr)[27])
{
    u32 vertCount = mesh->vertexs.size();
    u32 vertCountMinus_1 = vertCount - 1;//index 3
    u32 vertCountMinus_3 = vertCount - 3;//index 1
    mesh->triangles.m_vector.insert(mesh->triangles.m_vector.end(), {
        vertCount - 4, vertCountMinus_3,vertCountMinus_1, vertCount - 2,
        vertCountMinus_1,vertCountMinus_3 });

    u8 aos[4];
    u8 lightFace[4];
    getAO(aos, directFace, voxSurr);
    getVertexLightMaping(lightFace, directFace, lightSurr);
    glm::vec2 uvIndex[4] ={
        {0, 0},
        {0, 16},
        {16, 16},
        {16, 0},
    };
    for (u8 i = 0; i < 4; i++) {
        auto vert = &mesh->vertexs.m_vector[vertCount - (4 - i)];
        auto uv = vert->uv;
        vert->setAO(aos[i]);
        vert->setUV(glm::vec2(uv[0], uv[1]) + uvIndex[i]);
        vert->setLight(lightFace[i]);
    }
}
void ChunkSection::getLightSurround(uint8_t(&lights)[27], uint8_t x, uint8_t y, uint8_t z, 
    bool useFuncAccessOutChunk)
{
    uint8_t index = 0;
    if (useFuncAccessOutChunk) {
        for (int8_t j = y - 1; j <= y + 1; j++)
            for (int8_t k = z - 1; k <= z + 1; k++)
                for (int8_t i = x - 1; i <= x + 1; i++) {
                    lights[index++] = 15;
                }
        return;
    }
    for (int8_t j = y - 1; j <= y + 1; j++)
        for (int8_t k = z - 1; k <= z + 1; k++)
            for (int8_t i = x - 1; i <= x + 1; i++) {
                lights[index++] = 15;
            }
}
void ChunkSection::getAO(uint8_t(&refAO)[4], uint8_t directFace, uint8_t(&voxSurr)[27])
{
    if (directFace == 0)//Face up
    {
        refAO[0] = calculateAO(voxSurr[19], voxSurr[21], voxSurr[18]);
        refAO[1] = calculateAO(voxSurr[21], voxSurr[25], voxSurr[24]);
        refAO[2] = calculateAO(voxSurr[25], voxSurr[23], voxSurr[26]);
        refAO[3] = calculateAO(voxSurr[23], voxSurr[19], voxSurr[20]);
        return;
    }
    else if (directFace == 1)//down
    {
        refAO[0] = calculateAO(voxSurr[7], voxSurr[3], voxSurr[6]);
        refAO[1] = calculateAO(voxSurr[3], voxSurr[1], voxSurr[0]);
        refAO[2] = calculateAO(voxSurr[1], voxSurr[5], voxSurr[2]);
        refAO[3] = calculateAO(voxSurr[5], voxSurr[7], voxSurr[8]);
        return;
    }
    else if (directFace == 2)//north
    {
        refAO[0] = calculateAO(voxSurr[7], voxSurr[17], voxSurr[8]);
        refAO[1] = calculateAO(voxSurr[17], voxSurr[25], voxSurr[26]);
        refAO[2] = calculateAO(voxSurr[25], voxSurr[15], voxSurr[24]);
        refAO[3] = calculateAO(voxSurr[15], voxSurr[7], voxSurr[6]);
        return;
    }
    else if (directFace == 3)//south
    {
        refAO[0] = calculateAO(voxSurr[1], voxSurr[9], voxSurr[0]);
        refAO[1] = calculateAO(voxSurr[9], voxSurr[19], voxSurr[18]);
        refAO[2] = calculateAO(voxSurr[19], voxSurr[11], voxSurr[20]);
        refAO[3] = calculateAO(voxSurr[11], voxSurr[1], voxSurr[2]);
        return;
    }
    else if (directFace == 4)//east
    {
        refAO[0] = calculateAO(voxSurr[5], voxSurr[11], voxSurr[2]);
        refAO[1] = calculateAO(voxSurr[11], voxSurr[23], voxSurr[20]);
        refAO[2] = calculateAO(voxSurr[23], voxSurr[17], voxSurr[26]);
        refAO[3] = calculateAO(voxSurr[17], voxSurr[5], voxSurr[8]);
        return;
    }
    //west
    refAO[0] = calculateAO(voxSurr[3], voxSurr[15], voxSurr[6]);
    refAO[1] = calculateAO(voxSurr[15], voxSurr[21], voxSurr[24]);
    refAO[2] = calculateAO(voxSurr[21], voxSurr[9], voxSurr[18]);
    refAO[3] = calculateAO(voxSurr[9], voxSurr[3], voxSurr[0]);
    return;
}
void ChunkSection::getVertexLightMaping(uint8_t(&refLight)[4], uint8_t directFace, uint8_t(&lightSurr)[27])
{
    if (directFace == 0) {//up
        refLight[0] = lightSurr[22];
        refLight[1] = lightSurr[22];
        refLight[2] = lightSurr[22];
        refLight[3] = lightSurr[22];
        return;

    }
    else if (directFace == 1) {//down
        refLight[0] = lightSurr[4];
        refLight[1] = lightSurr[4];
        refLight[2] = lightSurr[4];
        refLight[3] = lightSurr[4];
        return;

    }
    else if (directFace == 2) {//north
        refLight[0] = lightSurr[16];
        refLight[1] = lightSurr[16];
        refLight[2] = lightSurr[16];
        refLight[3] = lightSurr[16];
        return;

    }
    else if (directFace == 3) {//south
        refLight[0] = lightSurr[10];
        refLight[1] = lightSurr[10];
        refLight[2] = lightSurr[10];
        refLight[3] = lightSurr[10];
        return;

    }
    else if (directFace == 4) {//east
        refLight[0] = lightSurr[14];
        refLight[1] = lightSurr[14];
        refLight[2] = lightSurr[14];
        refLight[3] = lightSurr[14];
        return;
    }
    //west
    refLight[0] = lightSurr[12];
    refLight[1] = lightSurr[12];
    refLight[2] = lightSurr[12];
    refLight[3] = lightSurr[12];
}
bool ChunkSection::checkBlockIsShape(Shape shapeType, int direction,
    int x, int y, int z) {
    auto& models = BlockDatabase::GetInstance()->m_models;
    if (direction == 0) {
        if (y == CHUNK_SIZE_INDEX) {
            if (m_index == CHUNK_SIZE_INDEX) return false;

            return models[chunk->m_chunks[m_index + 1]->m_blocks[(z << 8) + x].type]->m_shape == shapeType;
        }
        return models[m_blocks[(z << 8) + ((y + 1) << 4) + x].type]->m_shape == shapeType;
    }
    else if (direction == 1) {
        if (y == 0) {
            if (m_index == 0) return false;

            return models[chunk->m_chunks[m_index - 1]->m_blocks[(z << 8) + (CHUNK_SIZE_INDEX << 4) + x].type]->m_shape == shapeType;;
        }
        return models[m_blocks[(z<< 8) + ((y-1) << 4) + x].type]->m_shape == shapeType;
    }
    else if (direction == 2) {
        if (z == CHUNK_SIZE_INDEX) {
            if (chunk->north == nullptr) return true;

            return  models[chunk->north->m_chunks[m_index]->m_blocks[(y << 4) + x].type]->m_shape == shapeType;
        }
        return models[m_blocks[((z + 1) << 8) + (y << 4) + x].type]->m_shape == shapeType;
    }
    else if (direction == 3) {
        if (z == 0) {
            if (chunk->south == nullptr) return true;

            return  models[chunk->south->m_chunks[m_index]->m_blocks[(CHUNK_SIZE_INDEX << 8) + (y << 4) + x].type]->m_shape == shapeType;;
        }
        return models[m_blocks[((z-1) << 8) + (y << 4) + x].type]->m_shape == shapeType;

    }
    else if (direction == 4) {
        if (x == CHUNK_SIZE_INDEX) {
            if (chunk->east == nullptr) return true;

            return  models[chunk->east->m_chunks[m_index]->m_blocks[(z << 8) + (y << 4)].type]->m_shape == shapeType;;
        }
        return models[m_blocks[(z << 8) + (y << 4) + (x + 1)].type]->m_shape == shapeType;
    }
    //else direction 5
    if (x == 0) {
        if (chunk->west == nullptr) return true;
        return models[chunk->west->m_chunks[m_index]->m_blocks[(z << 8) + (y << 4) + CHUNK_SIZE_INDEX].type]->m_shape == shapeType;
    }
    return models[m_blocks[(z << 8) + (y << 4) + (x-1)].type]->m_shape == shapeType;
}
uint8_t ChunkSection::calculateAO(uint8_t side1, uint8_t side2, uint8_t corner)
{
    if (side1 + side2 == 2) return 3;
    return side1 + side2 + corner;
}
Block* ChunkSection::tryGetBlock(int direction, int x, int y, int z)
{
    if (direction == 0) {
        if (y == CHUNK_SIZE_INDEX) {
            if (m_index == CHUNK_SIZE_INDEX) return nullptr;
            auto nxtCS = chunk->m_chunks[m_index + 1];
            return &nxtCS->m_blocks[(z << 8) + x];
        }
        y++;
    }
    else if (direction == 1) {
        if (y == 0) {
            if (m_index == 0) return nullptr;

            auto nxtCS = chunk->m_chunks[m_index - 1];
            return &nxtCS->m_blocks[(z << 8) + (CHUNK_SIZE_INDEX << 4) + x];
        }
        y--;
    }
    else if (direction == 2) {
        if (z == CHUNK_SIZE_INDEX) {
            if (chunk->north == nullptr) return nullptr;

            auto nxtCS = chunk->north->m_chunks[m_index];
            return &nxtCS->m_blocks[(y << 4) + x];
        }
        z++;
    }
    else if (direction == 3) {
        if (z == 0) {
            if (chunk->south == nullptr) return nullptr;

            auto nxtCS = chunk->south->m_chunks[m_index];
            return &nxtCS->m_blocks[(CHUNK_SIZE_INDEX << 8) + (y << 4) + x];
        }
        z--;
    }
    else if (direction == 4) {
        if (x == CHUNK_SIZE_INDEX) {
            if (chunk->east == nullptr) return nullptr;

            auto nxtCS = chunk->east->m_chunks[m_index];
            return &nxtCS->m_blocks[(z << 8) + (y << 4)];
        }
        x++;
    }
    else {
        if (x == 0) {
            if (chunk->west == nullptr) return nullptr;

            auto nxtCS = chunk->west->m_chunks[m_index];
            return &nxtCS->m_blocks[(z << 8) + (y << 4) + CHUNK_SIZE_INDEX];
        }
        x--;
    }

    return &m_blocks[(z << 8) + (y << 4) + x];
}
BlockModel* ChunkSection::tryGetBlockModel(int direction, int x, int y, int z)
{
    auto& models = BlockDatabase::GetInstance()->m_models;
    auto block = tryGetBlock(direction, x, y, z);
    if (block == nullptr) return nullptr;
    return models[block->type];
}
void ChunkSection::getBlockSurround(uint8_t(&blocks)[27], uint8_t x, uint8_t y, uint8_t z,
    bool useFuncAccessOutChunk) {
    u8 index = 0;
    // optimize
    // use function check next voxel of this chunk
    if (useFuncAccessOutChunk) {
        for (int j = y - 1; j <= y + 1; j++)
            for (int k = z - 1; k <= z + 1; k++)
                for (int i = x - 1; i <= x + 1; i++) {
                    int worldY = j + (m_index << 4);
                    auto block = chunk->getBlock({ i,worldY,k });
                    if (block == nullptr) {
                        blocks[index++] = 1;
                        continue;
                    };

                    blocks[index++] = block->type == 0 ? 0 : 1;
                }
        return;
    }

    //get voxel in chunk
    for (u8 j = y - 1; j <= y + 1; j++)
        for (u8 k = z - 1; k <= z + 1; k++)
            for (u8 i = x - 1; i <= x + 1; i++)
                blocks[index++] = m_blocks[(k << 8) + (j << 4) + i].type == 0 ? 0 : 1;
}