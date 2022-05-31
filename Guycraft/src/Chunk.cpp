#include <Chunk.h>
#include "ChunkMeshBuilding.h"
#include "ResourceManager.h"
#include "ChunkManager.h"
#include "BlockSystem/BlockDatabase.h"
#include <algorithm>
#include <iostream>
void Chunk::init() {
    for (int i = 0; i < CHUNK_SIZE; i++) {
        auto chunkSection = new ChunkSection();
        auto mesh = new MeshChunkSection();

        mesh->m_chunk = chunkSection;
        mesh->m_index = i;

        chunkSection->m_mesh = mesh;
        chunkSection->m_index = i;
        chunkSection->chunk = this;

        m_chunks[i] = chunkSection;
    }
}
std::vector<Chunk*> Chunk::getAllChunkNeighbor()
{
    return  { north,south, east,west,northEast, northWest, southEast,southWest };
}
void Chunk::changeBlocks(Block blocksReplace[CHUNK_BLOCK_ALL]) {
    for (int chunkIndex = 0; chunkIndex < CHUNK_SIZE; chunkIndex++) {
        auto cs = m_chunks[chunkIndex];
        for(int z = 0; z< CHUNK_SIZE;z++)
            for (int y = 0; y < CHUNK_SIZE; y++)
                for (int x = 0; x < CHUNK_SIZE; x++) {
                    int acc = (z << 8) + (y << 4) + x;
                    auto block = blocksReplace[acc + (chunkIndex << 12)];
                    cs->m_blocks[acc] = block;
                    if (block.type != 0) {
                        cs->m_isBlocksEmpty = false;
                    }
                }
    }
}
void Chunk::render(Shader* shaders[2]) {
    auto sdSolid = shaders[0];
    auto sdFluid = shaders[1];
    glm::vec3 position = { pos.x, 0.f, pos.y };
    glm::mat4 model = glm::mat4(1.f);
    std::vector<MeshChunkSection*> meshsActive;
    for (auto chunkSection : m_chunks) {
        auto mesh = chunkSection->m_mesh;
        if (mesh->isActive) {
            meshsActive.push_back(mesh);
        }
    }
    for (auto m : meshsActive) {
        auto chunkSection = m->m_chunk;
        glm::vec3 chunkSectionPos = chunkSection->pos;

        model = glm::mat4(1.f);
        model = glm::translate(model, chunkSectionPos);
        //render mesh solid
        sdSolid->Bind();
        sdSolid->SetMat4("model", model);
        m->solid.draw();
        sdSolid->UnBind();

        sdFluid->Bind();
        sdFluid->SetMat4("model", model);
        m->fluid.draw();
        sdFluid->UnBind();
    }
}
void Chunk::unload() {
    isLoad = false;
    m_meshsActive.m_map.clear();
    for (auto cs : m_chunks) {
        cs->unLoad();
    }

    mutexNeighbor.lock();
    if (northEast != nullptr) {
        northEast->southWest = nullptr;
        northEast = nullptr;
    }
    if (northWest != nullptr) {
        northWest->southEast = nullptr;
        northWest = nullptr;
    }
    if (southEast != nullptr) {
        southEast->northWest = nullptr;
        southEast = nullptr;
    }
    if (southWest != nullptr) {
        southWest->northEast = nullptr;
        southWest = nullptr;
    }
    if (north != nullptr) {
        north->south = nullptr;
        north = nullptr;
    }
    if (south != nullptr) {
        south->north = nullptr;
        south = nullptr;
    }
    if (east != nullptr) {
        east->west = nullptr;
        east = nullptr;
    }
    if (west != nullptr) {
        west->east = nullptr;
        west = nullptr;
    }
    mutexNeighbor.unlock();
}
void Chunk::newSetupViaChunkPooling(glm::ivec2 newPos)
{
    pos = newPos;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        m_chunks[i]->pos = { newPos.x, i << 4, newPos.y };
    }
}
void Chunk::onload() {
    isLoad = true;
    isShouldUnload = false;
};
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
Block* Chunk::getBlock(glm::ivec3 pos) {
    int x = pos.x;
    int y = pos.y;
    int z = pos.z;

    if (y > CHUNK_HEIGHT_INDEX or y < 0) { 
        return nullptr;
    }
    auto chunkCurrent = this;
    if (x < 0) {
        if (west == nullptr) return nullptr;
        chunkCurrent = west;
        x = CHUNK_SIZE_INDEX;
    }
    else if (x > CHUNK_SIZE_INDEX) {
        if (east == nullptr) return nullptr;
        chunkCurrent = east;
        x = 0;
    }

    if (z < 0) {
        if (chunkCurrent->south == nullptr) return nullptr;
        chunkCurrent = chunkCurrent->south;
        z = CHUNK_SIZE_INDEX;
    }
    else if (z > CHUNK_SIZE_INDEX) {
        if (chunkCurrent->north == nullptr) return nullptr;
        chunkCurrent = chunkCurrent->north;
        z = 0;
    }
    return &chunkCurrent->m_chunks[y / CHUNK_SIZE]->m_blocks[(z<<8) + ((y%CHUNK_SIZE)<<4) + x];
}
void Chunk::setBlock(glm::ivec3 pos, Block block) {
    int y = pos.y;
    m_chunks[y / CHUNK_SIZE]->m_blocks[pos.z << 8 + (y % CHUNK_SIZE << 4) + pos.x] = block;
}