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
	lastViewPos = CameraManager::GetCurrentCamera()->transform.position;
	chunkMeshBuilding.startWithThread();
	auto camera = CameraManager::GetCurrentCamera();
	auto posPlayerToChunk = ToChunkPosition(camera->transform.position);
	chunkLoader.firstLoader(posPlayerToChunk);
}
glm::vec3 ChunkManager::ToChunkPosition(glm::vec3 pos) const
{
	if (pos.x < 0.f) {
		pos.x -= 32;
	}
	pos.y = glm::clamp(pos.y, 0.f, 255.f);

	if (pos.z < 0.f)
		pos.z -= 32;

	int x = floor(pos.x);
	int y = floor(pos.y);
	int z = floor(pos.z);

	return glm::vec3(
		(x / (int)CHUNK_SIZE) << 5,
		(y / (int)CHUNK_SIZE) << 5,
		(z / (int)CHUNK_SIZE) << 5);
}
void ChunkManager::update() {
	auto camera = CameraManager::GetCurrentCamera();
	auto posCamera = camera->transform.position;
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
	auto sdSolid = res->m_shaders["chunk_block_solid"];
	auto sdFulid = res->m_shaders["chunk_block_fluid"];
	sdSolid->Bind();
	sdSolid->SetVar("tex", 0);
	sdSolid->SetFloat("aoStrength", 0.45f);

	sdFulid->Bind();
	sdFulid->SetVar("tex", 0);
	sdFulid->SetFloat("aoStrength", 0.45f);
	sdFulid->SetFloat("time", (float)Time::lastTime);

	CameraManager::GetInstance().uploadCameraMatrixToShader(sdSolid);
	CameraManager::GetInstance().uploadCameraMatrixToShader(sdFulid);

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