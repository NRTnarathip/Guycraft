#include "ChunkLoader.h"
#include "ChunkManager.h"
#include "ClientEngine.h"

void ChunkLoader::firstLoader(glm::ivec2 pos) {
	lastPosChunk = pos;
	m_queueRequestLoadChunk.lock();
	m_queueRequestLoadChunk.push(pos);
	m_queueRequestLoadChunk.unlock();
}
void ChunkLoader::update(glm::ivec2 posPlayer) {
	if (lastPosChunk != posPlayer) {
		lastPosChunk = posPlayer;
		onPlayerMoveToNewChunk();
	}

	m_queueRequestLoadChunk.lock();
	if (not m_queueRequestLoadChunk.empty()) {
		auto cManager = ChunkManager::GetInstance();
		auto pos = m_queueRequestLoadChunk.getFront();
		cManager->chunkGroups.lock();
		if (not cManager->isExistChunkGroup(pos)) {
			if (cManager->ChunkInRange(posPlayer, pos)) {
				auto newChunkGroup = cManager->newChunkGroup(pos.x, pos.y);
				glm::ivec2 posNextChunk[4] = {
					glm::ivec2(0, Chunk::CHUNK_SIZE) + pos,
					glm::ivec2(0, -Chunk::CHUNK_SIZE) + pos,
					glm::ivec2(Chunk::CHUNK_SIZE, 0) + pos,
					glm::ivec2(-Chunk::CHUNK_SIZE, 0) + pos,
				};
				m_queueRequestLoadChunk.push(posNextChunk[0]);
				m_queueRequestLoadChunk.push(posNextChunk[1]);
				m_queueRequestLoadChunk.push(posNextChunk[2]);
				m_queueRequestLoadChunk.push(posNextChunk[3]);
			}
		}
		cManager->chunkGroups.unlock();
	}
	m_queueRequestLoadChunk.unlock();
}

void ChunkLoader::onPlayerMoveToNewChunk()
{
	printf("On player move to new chunk\n");

}
