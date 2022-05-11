#pragma once
#include <glm/glm.hpp>
#include "SmartQueue.h"

class ChunkLoader {
public:
	glm::ivec2 lastPosChunk;
	SmartQueue<glm::ivec2> m_queueRequestLoadChunk;
	void firstLoader(glm::ivec2 pos);
	void update(glm::ivec2 pos);
	void onPlayerMoveToNewChunk();
};