#pragma once
#include "ChunkContainer.h"
#include "SmartQueue.h"

class ChunkLoader {
public:
	ChunkContainer m_requestChunks;
	SmartQueue<Chunk*> m_queueNewRequestChunk;
	SmartQueue<Chunk*> m_queueRequestCompleteChunk;

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