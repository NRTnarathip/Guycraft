#include "ChunkLoader.h"
#include "ChunkManager.h"
#include "ClientEngine.h"
#include "CameraManager.h"
#include <map>
#include <unordered_map>

void useThreadPopulate() {
	auto cManager = ChunkManager::GetInstance();
	TerrainGen terrainGen;
	auto cLoader = &cManager->chunkLoader;
	auto m_queueNewRequestChunk = &cManager->chunkLoader.m_queueNewRequestChunk;
	while (true) {
		m_queueNewRequestChunk->lock();
		if (m_queueNewRequestChunk->empty()) {
			m_queueNewRequestChunk->unlock();
			continue;
		}
		auto smChunk = m_queueNewRequestChunk->getFront();
		m_queueNewRequestChunk->unlock();

		smChunk->lock();
		auto cg = smChunk->get();
		if (cg->isFourceUnload) {
			printf("dont populate on CG is fource unload\n");
			cManager->chunkPooling.collectPooling(smChunk);
			smChunk->unlock();
			continue;
		}
		terrainGen.populate(cg);
		smChunk->unlock();
		cLoader->m_queueRequestCompleteChunk.lock();
		cLoader->m_queueRequestCompleteChunk.push(smChunk);
		cLoader->m_queueRequestCompleteChunk.unlock();
	}
}
void ChunkLoader::startThread()
{
	m_threadPopulate = std::move(std::thread(useThreadPopulate));
}

void ChunkLoader::firstLoader(glm::ivec2 pos) {
	startThread();
	lastPosChunk = pos;
	progressLoadChunkGroup(pos);
}
void ChunkLoader::update(glm::ivec2 posPlayer) {
	if (lastPosChunk != posPlayer) {
		lastPosChunk = posPlayer;
		onPlayerMoveToNewChunk();
	}
	//create chunk from request loadchunk
	auto manager = ChunkManager::GetInstance();
	m_queueRequestCompleteChunk.lock();
	while(m_queueRequestCompleteChunk.size() > 0) {
		auto smChunk = m_queueRequestCompleteChunk.getFront();
		auto pos = smChunk->get()->pos;
		if (m_requestChunkGroup.exist(pos)) {
			if (manager->chunkGroups.exist(pos)) {
				manager->chunkPooling.collectPooling(manager->chunkGroups.m_container[pos]);
				manager->chunkGroups.m_container.erase(pos);
			}
			manager->chunkGroups.m_container.emplace(pos, smChunk);
			for (auto c : smChunk->get()->chunks) {
				manager->chunkMeshBuilding.addQueue(c);
			}
		}
		else {
			unloadChunkGroup(smChunk);
			m_requestChunkGroup.m_container.erase(pos);
		}
	}
	m_queueRequestCompleteChunk.unlock();
}
void ChunkLoader::onPlayerMoveToNewChunk()
{
	auto cManager = ChunkManager::GetInstance();
	std::vector<SmartChunkGroup*> listUnloadChunkGroup;
	for (auto &elem : cManager->chunkGroups.m_container) {
		auto smChunkGroup = elem.second;
		smChunkGroup->lock();
		auto posChunk = smChunkGroup->get()->pos;
		if (not ChunkManager::ChunkInRange(lastPosChunk, posChunk)) {
			listUnloadChunkGroup.push_back(smChunkGroup);
			if (m_requestChunkGroup.exist(posChunk)) {
				m_requestChunkGroup.m_container.erase(posChunk);
			}
		}
		smChunkGroup->unlock();
	}
	for (auto smChunkGroup : listUnloadChunkGroup) {
		smChunkGroup->get()->isFourceUnload = true;
		unloadChunkGroup(smChunkGroup);
		auto pos = smChunkGroup->get()->pos;
	}
	listUnloadChunkGroup.clear();
	progressLoadChunkGroup(lastPosChunk);
}

void ChunkLoader::unloadChunkGroup(SmartChunkGroup* smChunkGroup)
{
	auto cm = ChunkManager::GetInstance();
	auto pos = smChunkGroup->get()->pos;
	cm->chunkGroups.m_container.erase(pos);
	smChunkGroup->get()->isFourceUnload = true;
	cm->chunkPooling.collectPooling(smChunkGroup);
	for (auto c : smChunkGroup->get()->chunks) {
		c->isFourceStopGenerateMesh = true;
	}
	//check packet req queue and delete
	
}

void ChunkLoader::progressLoadChunkGroup(glm::ivec2 posInit)
{
	std::queue<glm::ivec2> queueLoadChunk;
	queueLoadChunk.push(posInit);

	std::unordered_map<glm::ivec2, bool> m_containerPosProgress;
	m_containerPosProgress.emplace(posInit, false);
	auto cManager = ChunkManager::GetInstance();
	while (queueLoadChunk.size() > 0) {
		auto pos = queueLoadChunk.front();
		queueLoadChunk.pop();
		if (ChunkManager::ChunkInRange(lastPosChunk, pos)) {
			if (not m_requestChunkGroup.exist(pos)) {
				m_requestChunkGroup.m_container.emplace(pos, nullptr);
				auto smChunk = cManager->chunkPooling.makeSmartChunk(pos);
				m_queueNewRequestChunk.lock();
				m_queueNewRequestChunk.push(smChunk);
				m_queueNewRequestChunk.unlock();
			}
			glm::ivec2 posNextChunk[4] = {
				glm::ivec2(0, Chunk::CHUNK_SIZE) + pos,
				glm::ivec2(0, -Chunk::CHUNK_SIZE) + pos,
				glm::ivec2(Chunk::CHUNK_SIZE, 0) + pos,
				glm::ivec2(-Chunk::CHUNK_SIZE, 0) + pos,
			};
			for (auto posNext : posNextChunk) {
				if (m_containerPosProgress.find(posNext) == m_containerPosProgress.end()) {
					if (ChunkManager::ChunkInRange(lastPosChunk, posNext)) {
						queueLoadChunk.push(posNext);
						m_containerPosProgress.emplace(posNext, true);
					}
				}
			}
		}
	}
}
