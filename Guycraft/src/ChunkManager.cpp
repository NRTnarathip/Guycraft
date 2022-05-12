#include <ChunkManager.h>
#include <Game.h>
#include "glm/gtx/string_cast.hpp"
#include <array>
#include <SmartQueue.h>
#include "CameraManager.h"
#include "ClientEngine.h"

ChunkManager* ChunkManager::m_instance = nullptr;
bool inRange(int value, int min, int max)
{
	return (value >= min) and (value <= max);
}
void ChunkManager::init() {
	lastViewPos = CameraManager::GetCurrentCamera()->Postition;
	chunkMeshBuilding.startWithThread();
	auto camera = CameraManager::GetCurrentCamera();
	auto posPlayerToChunk = ToChunkPosition(camera->Postition);
	chunkLoader.firstLoader(posPlayerToChunk);
}
void ChunkManager::initChunkNear(SmartChunkGroup *smChunk, SmartChunkGroup* cgNearNorth,
	SmartChunkGroup* cgNearSouth, SmartChunkGroup* cgNearEast, SmartChunkGroup* cgNearWest) {
	auto cg = smChunk->get();

	cg->nearEast = cgNearEast->get();
	cg->nearWest = cgNearWest->get();
	cg->nearSouth = cgNearSouth->get();
	cg->nearNorth = cgNearNorth->get();

	for (int y = 0; y < 8; y++) {
		Chunk* c = cg->chunks[y];
		if (y >= 1) {
			c->cnearDown = cg->chunks[y - 1];
		}

		if (y <= 6) {
			c->cnearUp = cg->chunks[y + 1];
		}

		if (cgNearNorth != NULL) {
			c->cnearNorth = cgNearNorth->get()->chunks[y];
		}
		
		if (cgNearSouth != NULL) {
			c->cnearSouth = cgNearSouth->get()->chunks[y];
		}
		if (cgNearEast != NULL) {
			c->cnearEast = cgNearEast->get()->chunks[y];
		}
		if (cgNearWest != NULL) {
			c->cnearWest = cgNearWest->get()->chunks[y];
		}
	}
}
glm::ivec3 ChunkManager::ToChunkPosition(glm::vec3 pos) const
{
	if (pos.x < 0.f) {
		pos.x -= Chunk::CHUNK_SIZE;
	}

	if (pos.z < 0.f)
		pos.z -= Chunk::CHUNK_SIZE;

	int x = (int)pos.x;
	int y = (int)pos.y;
	int z = (int)pos.z;

	return glm::ivec3(
		(x / (int)Chunk::CHUNK_SIZE) * Chunk::CHUNK_SIZE,
		(y / (int)Chunk::CHUNK_SIZE) * Chunk::CHUNK_SIZE,
		(z / (int)Chunk::CHUNK_SIZE) * Chunk::CHUNK_SIZE);
}
void ChunkManager::update() {
	auto camera = CameraManager::GetCurrentCamera();
	auto posCamera = camera->Postition;
	auto posPlayerToChunk = ToChunkPosition(posCamera);

	//check if chunk is not use or out of range render system
	chunkLoader.update({ posPlayerToChunk.x, posPlayerToChunk.z });
}
bool ChunkManager::ChunkInRange(glm::ivec2 playerPos, glm::ivec2 chunkPos)
{
	int rangeRender = ClientEngine::GetInstance().graphicSetting.renderDistance * Chunk::CHUNK_SIZE;
	int offsX = chunkPos.x - playerPos.x;
	int offsZ = chunkPos.y - playerPos.y;
	if (inRange(offsX, -rangeRender, rangeRender)
		and inRange(offsZ, -rangeRender, rangeRender)) {
		return true;
	}
	return false;
}
void ChunkManager::render() {
	auto camera = CameraManager::GetCurrentCamera();
	auto res = ResourceManager::GetInstance();
	auto shaderChunkSolid = res->m_shaders["chunk_block_solid"];
	shaderChunkSolid->Bind();
	shaderChunkSolid->SetVar("tex", 0);
	shaderChunkSolid->SetFloat("aoStrength", 0.45f);
	CameraManager::GetInstance().uploadCameraMatrixToShader(shaderChunkSolid);

	for (auto kvp : chunkGroups.m_container) {
		auto smartChunk = kvp.second;
		smartChunk->get()->render();
	}
}
SmartChunkGroup* ChunkManager::getChunkGroup(glm::ivec2 pos) {
	if (chunkGroups.exist(pos)) {
		return chunkGroups.m_container.at(pos);
	}
	return nullptr;
};