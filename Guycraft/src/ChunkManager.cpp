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
	auto queDel = &chManager->queDeleteChunk;
	auto chunkPooling = &chManager->chunkPooling;
	while (true) {
		auto smChunk = queDel->getFront();
		if (smChunk == NULL) continue;

		smChunk->lock();

		chunkPooling->collectPooling(smChunk);

		smChunk->unlock();
	}
}
void useThreadPopulate() {
	auto chManager = ChunkManager::GetInstance();
	auto terrainGen = chManager->terrainGen;
	while (true) {

		auto smChunk = chManager->queNeedPopulate.getFront();
		if (smChunk == NULL) continue;

		smChunk->lock();
		auto cg = smChunk->get();
		terrainGen.populate(cg);
		cg->needGenMeshChunk = true;

		smChunk->unlock();

		chManager->queNeedGenMeshChunkGroup.push(smChunk);
	}
}
bool inRange(int x, int min, int max)
{
	return (x >= min) and (x <= max);
}
void ChunkManager::init() {
	lastViewPos = CameraManager::GetCurrentCamera()->Postition;
	std::thread th;
	th = std::thread(useThreadPopulate);

	std::thread thDeleteChunk;
	thDeleteChunk = std::thread(useThreadDelete);

	listThread.push_back(std::move(th));
	listThread.push_back(std::move(thDeleteChunk));

	genMeshChunk = new GenMeshChunk();
	genMeshChunk->init();
}
void ChunkManager::initChunkNear(SmartChunkGroup *smChunk, SmartChunkGroup* cgNearNorth,
	SmartChunkGroup* cgNearSouth, SmartChunkGroup* cgNearEast, SmartChunkGroup* cgNearWest) {
	auto cg = smChunk->get();

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

	if (cgNearNorth != NULL) {
		if (cgNearNorth->get()->needGenMeshChunk == false) {
			cgNearNorth->get()->needGenMeshChunk = true;
			queNeedGenMeshChunkGroup.push(cgNearNorth);
		}
	}
	if (cgNearSouth != NULL) {
		if (cgNearSouth->get()->needGenMeshChunk == false) {
			cgNearSouth->get()->needGenMeshChunk = true;
			queNeedGenMeshChunkGroup.push(cgNearSouth);
		}

	}
	if (cgNearEast != NULL) {
		if (cgNearEast->get()->needGenMeshChunk == false) {
			cgNearEast->get()->needGenMeshChunk = true;
			queNeedGenMeshChunkGroup.push(cgNearEast);
		}

	}
	if (cgNearWest != NULL) {
		if (cgNearWest->get()->needGenMeshChunk == false) {
			cgNearWest->get()->needGenMeshChunk = true;
			queNeedGenMeshChunkGroup.push(cgNearWest);
		}
	}
}
SmartChunkGroup* ChunkManager::newChunkGroup(int x,int z) {
	auto getCG = getChunkGroup(x, z);
	if (getCG != NULL) {
		//found chunk 
		return getCG;
	} 

	//else not found chunk in container

	auto pos = glm::ivec2(x, z);
	//pooling
	auto smartChunk = chunkPooling.makeSmartChunk(pos);
	chunkGroups[pos] = smartChunk;
	
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
bool ChunkManager::checkShouldNewChunk(glm::ivec3 posCamera) {
	float distX = posCamera.x - lastViewPos.x;
	float distZ = posCamera.z - lastViewPos.z;

	int distMinitudeNewSpawnChunk = Chunk::CHUNK_SIZE * 2;

	bool isShouldSpawnNewChunk = false;
	if (not inRange(distX ,-distMinitudeNewSpawnChunk, distMinitudeNewSpawnChunk)
		or not inRange(distZ ,-distMinitudeNewSpawnChunk, distMinitudeNewSpawnChunk)) {
		isShouldSpawnNewChunk = true;
	}
	return isShouldSpawnNewChunk;
}
void ChunkManager::update() {
	auto camera = CameraManager::GetCurrentCamera();
	printf("pos cam y: %f\n", camera->Postition.y);

	auto posCamera = camera->Postition;
	unsigned char distRender = ClientEngine::GetInstance().graphicSetting.renderDistance;
	// Create initial chunks
	auto posPlayerToChunk = ToChunkPosition(posCamera);
	if (chunkGroups.size() > 0) {
		if (not checkShouldNewChunk(posPlayerToChunk)) {
			return;
		}
	}
	
	lastViewPos = posPlayerToChunk;
	//render between -distance render to distance render
	//example dist = 3. we will spawn chunk at -3,-2,-1, 0, 1, 2, 3
	//int renderZ = -distRender; renderZ <= distRender;

	float counterTime = glfwGetTime();
	int minmaxRenderPos = distRender * Chunk::CHUNK_SIZE;

	for (int renderZ = -distRender; renderZ <= distRender; renderZ++) { 
		for (int renderX = -distRender; renderX <= distRender; renderX++) {
			newChunkGroup(posPlayerToChunk.x + (renderX*Chunk::CHUNK_SIZE), 
				posPlayerToChunk.z + (renderZ * Chunk::CHUNK_SIZE));
		}
	}
	std::deque<SmartChunkGroup*> queDeleteCG;
	for (auto it = chunkGroups.begin(); it != chunkGroups.end();)
	{
		auto smChunk = it->second;
		if (inRange(smChunk->get()->pos.x, -minmaxRenderPos + posPlayerToChunk.x, minmaxRenderPos + posPlayerToChunk.x) and
			inRange(smChunk->get()->pos.y, -minmaxRenderPos + posPlayerToChunk.z, minmaxRenderPos + posPlayerToChunk.z)) {
			++it;
			smChunk->get()->inRangeRender = true;
		}
		else {
			smChunk->get()->inRangeRender = false;
			chunkGroups.erase(it++);
			queDeleteCG.push_back(smChunk);
		}
	}

	//clear referance near chunk all
	while (queDeleteCG.size() > 0) {
		auto sm = queDeleteCG.front();
		queDeleteCG.pop_front();

		sm->lock();
		sm->get()->clearChunk();
		sm->unlock();

		queDeleteChunk.push(sm);
	}
	//clear
	std::deque<SmartChunkGroup*>().swap(queDeleteCG);

	//initial cnear chunk all
	while (queSpawnChunkGroup.size() > 0) {
		auto smChunk = queSpawnChunkGroup.getFront();

		auto cg = smChunk->get();

		//should init chunk near all
		float cgPosX = cg->pos.x;
		float cgPosZ = cg->pos.y;
		auto cgNearNorth = getChunkGroup(cgPosX, cgPosZ + Chunk::CHUNK_SIZE);
		auto cgNearSouth = getChunkGroup(cgPosX, cgPosZ - Chunk::CHUNK_SIZE);
		auto cgNearEast = getChunkGroup(cgPosX + Chunk::CHUNK_SIZE, cgPosZ);
		auto cgNearWest = getChunkGroup(cgPosX - Chunk::CHUNK_SIZE, cgPosZ);
		//initChunkNear(smChunk, cgNearNorth, cgNearSouth, cgNearEast, cgNearWest);

		queNeedPopulate.push(smChunk);
	}

}
bool ChunkManager::ChunkInRange(glm::vec3 playerPos, glm::ivec2 chunkPos, int distRender) const
{
	int rangeRender = distRender * Chunk::CHUNK_SIZE;
	float distXChunk = playerPos.x - chunkPos.x;
	float distZChunk = playerPos.z - chunkPos.y;
	if (not inRange(-rangeRender, rangeRender, distXChunk)
		or inRange(-rangeRender,rangeRender, distZChunk)) {
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

	for (auto kvp : chunkGroups) {
		auto smartChunk = kvp.second;
		smartChunk->get()->render();
	}
}
SmartChunkGroup* ChunkManager::getChunkGroup(int x, int z) {
	glm::ivec2 pos = glm::ivec2(x, z);
	ChunkGroupContainer::iterator it = chunkGroups.find(pos);
	if (it != chunkGroups.end()) { //found chunk 
		return chunkGroups.at(pos);
	}
	return NULL;
};