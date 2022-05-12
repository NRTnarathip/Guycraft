#pragma once
#include "ChunkGroupContainer.h"
#include "SmartQueue.h"
#include "SmartChunk.h"

class ChunkLoader {
public:
	ChunkGroupContainer m_requestChunkGroup;
	SmartQueue<SmartChunkGroup*> m_queueNewRequestChunk;
	SmartQueue<SmartChunkGroup*> m_queueRequestCompleteChunk;

	glm::ivec2 lastPosChunk;
	std::thread m_threadPopulate;
	void startThread();
	void firstLoader(glm::ivec2 pos);
	void update(glm::ivec2 pos);
	void onPlayerMoveToNewChunk();
	//none safe thread, dont use in loop chunkGroups.container
	void unloadChunkGroup(SmartChunkGroup* smChunkGroup);

	void progressLoadChunkGroup(glm::ivec2 pos);
};