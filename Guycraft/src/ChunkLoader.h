#pragma once
#include "ChunkContainer.h"
#include "SmartQueue.h"
#include "SmartUnorderMap.h"

class ChunkLoader {
public:
	SmartUnorderMap<glm::ivec2, bool> m_allocateChunk;
	SmartQueue<glm::ivec2> m_queueJobPopulate;
	SmartQueue<JobPopulate*> m_queueJobPopulateComplete;
	glm::ivec2 lastPosChunk;
	std::thread m_threadPopulate;

	void startThread();
	void firstLoader(glm::ivec2 pos);
	void update(glm::ivec2 pos);
	void onPlayerMoveToNewChunk();
	//none safe thread, dont use in loop chunkGroups.container
	void unloadChunk(Chunk* chunk);
	void progressLoadChunk(glm::ivec2 pos);
};