#include <ChunkGroup.h>
#include <Game.h>
ChunkGroup::ChunkGroup(glm::ivec2 initPos) {
    pos = initPos;
}

ChunkGroup::~ChunkGroup() {
    for (unsigned char i = 0; i < 8; i++) {
        delete chunks[i];
    }
}
void ChunkGroup::linkNeighborChunk(ChunkGroup* cgNeighbor[4]) {
    if (cgNeighbor[0]) {
        nearNorth = cgNeighbor[0];
        nearNorth->nearSouth = this;

    }
    if (cgNeighbor[1]) {
        nearSouth = cgNeighbor[1];
        nearSouth->nearNorth = this;
    }
    if (cgNeighbor[2]) {
        nearEast = cgNeighbor[2];
        nearEast->nearWest = this;
    }
    if (cgNeighbor[3]) {
        nearWest = cgNeighbor[3];
        nearWest->nearEast = this;
    }
}
void ChunkGroup::unlinkNeighborChunk() {
    if (nearNorth) {
        nearNorth->nearSouth = nullptr;
        nearNorth = nullptr;
    }
    if (nearSouth) {
        nearSouth->nearNorth = nullptr;
        nearSouth = nullptr;
    }
    if (nearEast) {
        nearEast->nearWest = nullptr;
        nearEast = nullptr;
    }
    if (nearWest) {
        nearWest->nearEast = nullptr;
        nearWest = nullptr;
    }
}
void ChunkGroup::fourceUnload() {
    isFourceUnload = true;
}

void ChunkGroup::clearChunk() {
    for (auto c : chunks) {
        //clear cnear referance all
        if (c->cnearNorth != NULL ) {
            c->cnearNorth->cnearSouth = NULL;
            c->cnearNorth = NULL;
        }
        if (c->cnearSouth != NULL) {
            c->cnearSouth->cnearNorth = NULL;
            c->cnearSouth = NULL;
        }
        if (c->cnearEast != NULL) {
            c->cnearEast->cnearWest = NULL;
            c->cnearEast = NULL;
        }
        if (c->cnearWest != NULL) {
            c->cnearWest->cnearEast = NULL;
            c->cnearWest = NULL;
        }
        c->clearAll();
    }
}
bool ChunkGroup::checkIsHaveVoxel(Chunk* c) {
    for (int acc = 0; acc < Chunk::CHUNK_SIZE_BLOCK; acc++) {
        if (c->voxels[acc].type > 0) {
            return true;
        }
    }
    return false;
}
void ChunkGroup::render() {
    auto resource = ResourceManager::GetInstance();
    auto defaultShader = resource->m_shaders["chunk_block_solid"];
    auto mcatlas = resource->m_textures["assets/textures/blocks/mcatlas.png"];
    defaultShader->Bind();
    for (auto c : chunks) {
        // Mesh must be on gpu to draw
        glm::vec3 pos;
        pos.x = c->pos.x;
        pos.y = c->pos.y;
        pos.z = c->pos.z;

        glm::mat4 model = glm::mat4(1.f);
        model = glm::translate(model, pos);
        defaultShader->SetMat4("model", model);
        mcatlas->Activate(GL_TEXTURE0);
        c->mesh.draw();
    }
}

void ChunkGroup::needGenMeshNear()
{
    auto cManager = ChunkManager::GetInstance();
    if (nearNorth != NULL) {
        if (nearNorth->needGenMeshChunk == false) {
            nearNorth->needGenMeshChunk = true;
            //cManager->genMeshChunk->enqueGenerate(nearNorth->sm);
        }
    }
    if (nearSouth != NULL) {
        if (nearSouth->needGenMeshChunk == false) {
            nearSouth->needGenMeshChunk = true;
            //cManager->genMeshChunk->enqueGenerate(nearSouth->sm);
        }

    }
    if (nearWest != NULL) {
        if (nearWest->needGenMeshChunk == false) {
            nearWest->needGenMeshChunk = true;
            //cManager->genMeshChunk->enqueGenerate(nearWest->sm);
        }
    }
    if (nearEast != NULL) {
        if (nearEast->needGenMeshChunk == false) {
            nearEast->needGenMeshChunk = true;
            //cManager->genMeshChunk->enqueGenerate(nearEast->sm);
        }
    }
}
