#pragma once
#include <glm/glm.hpp>
#include "SmartQueue.h"

class ChunkLoader {
public:
	glm::ivec2 lastPosChunk;
	void firstLoader(glm::ivec2 pos);
	void update(glm::ivec2 pos);
	void onPlayerMoveToNewChunk();
	//none safe thread, dont use in loop chunkGroups.container
	void unloadChunkGroup(SmartChunkGroup* smChunkGroup);

	void progressLoadChunkGroup(glm::ivec2 pos);
};