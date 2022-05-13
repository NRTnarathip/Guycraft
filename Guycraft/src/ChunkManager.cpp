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
glm::ivec3 ChunkManager::ToChunkPosition(glm::vec3 pos) const
{
	if (pos.x < 0.f) {
		pos.x -= CHUNK_SIZE;
	}

	if (pos.z < 0.f)
		pos.z -= CHUNK_SIZE;

	int x = (int)pos.x;
	int y = (int)pos.y;
	int z = (int)pos.z;

	return glm::ivec3(
		(x / (int)CHUNK_SIZE) * CHUNK_SIZE,
		(y / (int)CHUNK_SIZE) * CHUNK_SIZE,
		(z / (int)CHUNK_SIZE) * CHUNK_SIZE);
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
	int rangeRender = ClientEngine::GetInstance().graphicSetting.renderDistance * CHUNK_SIZE;
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

	for (auto kvp : chunks.m_container) {
		auto chunk = kvp.second;
		chunk->render();
	}
}
Chunk* ChunkManager::getChunk(glm::ivec2 pos) {
	if (chunks.exist(pos)) {
		return chunks.m_container.at(pos);
	}
	return nullptr;
};