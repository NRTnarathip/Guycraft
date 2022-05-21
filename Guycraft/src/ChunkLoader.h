#pragma once
#include "ChunkContainer.h"
#include "SmartQueue.h"
#include "SmartUnorderMap.h"

class ChunkLoader {
public:
	const glm::ivec2 chunkNeighborPositions[8] = {
				glm::ivec2(0, CHUNK_SIZE),//north
				glm::ivec2(0, -CHUNK_SIZE),//south
				glm::ivec2(CHUNK_SIZE, 0),//east
				glm::ivec2(-CHUNK_SIZE, 0),//west
				glm::ivec2(CHUNK_SIZE, CHUNK_SIZE),//north east
				glm::ivec2(CHUNK_SIZE, -CHUNK_SIZE),//south east
				glm::ivec2(-CHUNK_SIZE, -CHUNK_SIZE),//south west
				glm::ivec2(-CHUNK_SIZE, CHUNK_SIZE),//north west
	};

	SmartUnorderMap<glm::ivec2, bool> m_allocateChunk;
	SmartQueue<glm::ivec2> m_queueJobPopulate;
	SmartQueue<JobPopulate*> m_queueJobPopulateComplete;
	glm::ivec2 lastPosChunk;
	std::thread m_threadPopulate;

	void startThread();
	void firstLoader(glm::ivec2 pos);
	void update(glm::ivec2 pos);
	//none safe thread
	int getAllocateChunkNeighborCount(Chunk* chunk);
	void onPlayerMoveToNewChunk();
	//none safe thread, dont use in loop chunkGroups.container
	void unloadChunk(Chunk* chunk);
	void progressLoadChunk(glm::ivec2 pos);
};