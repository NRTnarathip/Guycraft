#include "ChunkLoader.h"
#include "ChunkManager.h"
#include "ClientEngine.h"
#include "CameraManager.h"
#include <map>
#include <unordered_map>

void ChunkLoader::firstLoader(glm::ivec2 pos) {
	lastPosChunk = pos;
	progressLoadChunkGroup(pos);
}
void ChunkLoader::update(glm::ivec2 posPlayer) {
	if (lastPosChunk != posPlayer) {
		lastPosChunk = posPlayer;
		onPlayerMoveToNewChunk();
	}
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
		}
		smChunkGroup->unlock();
	}
	for (auto smChunkGroup : listUnloadChunkGroup) {
		smChunkGroup->lock();
		smChunkGroup->get()->isFourceUnload = true;
		unloadChunkGroup(smChunkGroup);
		smChunkGroup->unlock();
	}
	listUnloadChunkGroup.clear();

	progressLoadChunkGroup(lastPosChunk);
}

void ChunkLoader::unloadChunkGroup(SmartChunkGroup* smChunkGroup)
{
	auto cm = ChunkManager::GetInstance();
	cm->chunkGroups.m_container.erase(smChunkGroup->get()->pos);
	cm->chunkPooling.collectPooling(smChunkGroup);
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
			if (not cManager->isExistChunkGroup(pos)) {
				cManager->newChunkGroup(pos.x, pos.y);
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
