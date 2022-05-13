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
		auto chunk = m_queueNewRequestChunk->getFront();
		m_queueNewRequestChunk->unlock();
		chunk->lock();
		terrainGen.populate(chunk);
		chunk->unlock();
		cLoader->m_queueRequestCompleteChunk.lock();
		cLoader->m_queueRequestCompleteChunk.push(chunk);
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
	progressLoadChunk(pos);
}
void ChunkLoader::update(glm::ivec2 posPlayer) {
	if (lastPosChunk != posPlayer) {
		lastPosChunk = posPlayer;
		onPlayerMoveToNewChunk();
	}
	//create chunk from request loadchunk
	auto manager = ChunkManager::GetInstance();
	manager->chunks.lock();
	m_queueRequestCompleteChunk.lock();
	while(m_queueRequestCompleteChunk.size() > 0) {
		auto chunk = m_queueRequestCompleteChunk.getFront();
		chunk->lock();
		auto pos = chunk->pos;
		if (m_requestChunks.exist(pos)) {
			if (manager->chunks.exist(pos)) {
				manager->chunkPooling.collectPooling(manager->chunks.m_container[pos]);
				manager->chunks.m_container.erase(pos);
			}
			//init chunk in client
			manager->chunks.m_container.emplace(pos, chunk);
			//setup chunk neighbor
			Chunk* chunksNeighbor[4] = {nullptr,nullptr,nullptr,nullptr};
			glm::ivec2 posChunkNeighbor[4] = {
				glm::ivec2(0, CHUNK_SIZE) + pos,
				glm::ivec2(0, -CHUNK_SIZE) + pos,
				glm::ivec2(CHUNK_SIZE, 0) + pos,
				glm::ivec2(-CHUNK_SIZE, 0) + pos,
			};
			for (int i = 0; i < 4; i++) {
				auto posNext = posChunkNeighbor[i];
				auto c = manager->getChunk(posNext);
				if (c != nullptr) {
					chunksNeighbor[i] = c;
				}
			}
			chunk->linkChunkNeighbor(chunksNeighbor);
			chunk->onLoad();
			manager->chunkMeshBuilding.addQueue(chunk);
		}
		else {
			unloadChunk(chunk);
			m_requestChunks.m_container.erase(pos);
		}
		chunk->unlock();
	}
	manager->chunks.unlock();
	m_queueRequestCompleteChunk.unlock();
}
void ChunkLoader::onPlayerMoveToNewChunk()
{
	auto cManager = ChunkManager::GetInstance();
	std::vector<Chunk*> listUnloadChunk;
	for (auto &elem : cManager->chunks.m_container) {
		auto chunk = elem.second;
		chunk->lock();
		auto posChunk = chunk->pos;
		if (not ChunkManager::ChunkInRange(lastPosChunk, posChunk)) {
			listUnloadChunk.push_back(chunk);
			if (m_requestChunks.exist(posChunk)) {
				m_requestChunks.m_container.erase(posChunk);
			}
		}
		chunk->unlock();
	}
	for (auto chunk : listUnloadChunk) {
		unloadChunk(chunk);
	}
	listUnloadChunk.clear();
	progressLoadChunk(lastPosChunk);
}

void ChunkLoader::unloadChunk(Chunk* chunk)
{
	//clear chunk mesh on gpu
	chunk->lock();
	chunk->unload();
	for (auto &mesh : chunk->meshs) {
		mesh.clearOnGPU();
	}
	chunk->unlock();
	auto cm = ChunkManager::GetInstance();
	auto pos = chunk->pos;
	cm->chunks.m_container.erase(pos);
	cm->chunkPooling.collectPooling(chunk);
}

void ChunkLoader::progressLoadChunk(glm::ivec2 posInit)
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
			if (not m_requestChunks.exist(pos)) {
				m_requestChunks.m_container.emplace(pos, nullptr);
				auto smChunk = cManager->chunkPooling.makeObject(pos);
				m_queueNewRequestChunk.lock();
				m_queueNewRequestChunk.push(smChunk);
				m_queueNewRequestChunk.unlock();
			}
			glm::ivec2 posNextChunk[4] = {
				glm::ivec2(0, CHUNK_SIZE) + pos,
				glm::ivec2(0, -CHUNK_SIZE) + pos,
				glm::ivec2(CHUNK_SIZE, 0) + pos,
				glm::ivec2(-CHUNK_SIZE, 0) + pos,
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
