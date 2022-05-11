#include <ChunkManager.h>
#include <Game.h>
#include "glm/gtx/string_cast.hpp"
#include <array>
#include <SmartQueue.h>
#include "CameraManager.h"
#include "ClientEngine.h"

ChunkManager* ChunkManager::m_instance = nullptr;
void useThreadDelete() {
	auto chManager = ChunkManager::GetInstance();
	auto queueUnload = &chManager->m_queueUnload;
	auto chunkPooling = &chManager->chunkPooling;
	auto m_chunkContainer = &chManager->chunkGroups;

	while (true) {
		queueUnload->lock();
		if (queueUnload->empty()) {
			queueUnload->unlock();
			continue;
		}
		auto smChunk = queueUnload->getFront();
		queueUnload->unlock();
		
		if (smChunk == NULL) continue;
		smChunk->lock();
		chunkPooling->collectPooling(smChunk);

		m_chunkContainer->lock();
		m_chunkContainer->get().erase(smChunk->get()->pos);
		m_chunkContainer->unlock();

		smChunk->unlock();
	}
}
void useThreadPopulate() {
	auto chManager = ChunkManager::GetInstance();
	TerrainGen terrainGen;
	while (true) {

		auto smChunk = chManager->queNeedPopulate.getFront();
		if (smChunk == NULL) continue;

		smChunk->lock();
		auto cg = smChunk->get();
		terrainGen.populate(cg);
		smChunk->unlock();

		for (auto c : cg->chunks) {
			c->isNeedGenerateMesh = true;
			chManager->chunkMeshBuilding.addQueue(c);
		}
	}
}
bool inRange(int value, int min, int max)
{
	return (value >= min) and (value <= max);
}
void ChunkManager::init() {
	lastViewPos = CameraManager::GetCurrentCamera()->Postition;
	chunkMeshBuilding.startWithThread();

	std::thread thPopulate;
	thPopulate = std::thread(useThreadPopulate);

	std::thread thDeleteChunk;
	thDeleteChunk = std::thread(useThreadDelete);


	listThread.push_back(std::move(thPopulate));
	listThread.push_back(std::move(thDeleteChunk));


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
SmartChunkGroup* ChunkManager::newChunkGroup(int x,int z) {
	auto getCG = getChunkGroup({x, z});
	if (getCG != nullptr) {
		return getCG;
	} 
	auto pos = glm::ivec2(x, z);
	auto smartChunk = chunkPooling.makeSmartChunk(pos);
	chunkGroups.get()[pos] = smartChunk;
	queSpawnChunkGroup.push(smartChunk);

	return smartChunk;
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

	//initial cnear chunk all
	while (queSpawnChunkGroup.size() > 0) {
		auto smChunk = queSpawnChunkGroup.getFront();

		auto cg = smChunk->get();

		//should init chunk near all
		float cgPosX = cg->pos.x;
		float cgPosZ = cg->pos.y;
		chunkGroups.lock();
		auto cgNearNorth = getChunkGroup({ cgPosX, cgPosZ + Chunk::CHUNK_SIZE });
		auto cgNearSouth = getChunkGroup({ cgPosX, cgPosZ - Chunk::CHUNK_SIZE });
		auto cgNearEast = getChunkGroup({ cgPosX + Chunk::CHUNK_SIZE, cgPosZ });
		auto cgNearWest = getChunkGroup({ cgPosX - Chunk::CHUNK_SIZE, cgPosZ });
		chunkGroups.unlock();
		//initChunkNear(smChunk, cgNearNorth, cgNearSouth, cgNearEast, cgNearWest);
		queNeedPopulate.push(smChunk);
	}
}

bool ChunkManager::isExistChunkGroup(glm::ivec2 pos) {
	ChunkGroupContainer::iterator it = chunkGroups.get().find(pos);
	if (it != chunkGroups.get().end()) //found chunk 
		return true;
	return false;
}
bool ChunkManager::ChunkInRange(glm::ivec2 playerPos, glm::ivec2 chunkPos)
{
	int rangeRender = ClientEngine::GetInstance().graphicSetting.renderDistance * Chunk::CHUNK_SIZE;
	int distXChunk = playerPos.x - chunkPos.x;
	int distZChunk = playerPos.y - chunkPos.y;
	if (not inRange(distXChunk, -rangeRender, rangeRender)
		or not inRange(distZChunk, -rangeRender,rangeRender)) {
		return false;
	}
	return true;
}
void ChunkManager::render() {
	auto camera = CameraManager::GetCurrentCamera();
	auto res = ResourceManager::GetInstance();
	auto shaderChunkSolid = res->m_shaders["chunk_block_solid"];
	shaderChunkSolid->Bind();
	shaderChunkSolid->SetVar("tex", 0);
	shaderChunkSolid->SetFloat("aoStrength", 0.45f);
	CameraManager::GetInstance().uploadCameraMatrixToShader(shaderChunkSolid);

	chunkGroups.lock();
	auto m_chunks = chunkGroups.get();
	for (auto kvp : m_chunks) {
		auto smartChunk = kvp.second;
		smartChunk->get()->render();
	}
	chunkGroups.unlock();
}
SmartChunkGroup* ChunkManager::getChunkGroup(glm::ivec2 pos) {
	ChunkGroupContainer::iterator it = chunkGroups.get().find(pos);
	if (it != chunkGroups.get().end()) { //found chunk 
		return chunkGroups.get().at(pos);
	}
	return nullptr;
};