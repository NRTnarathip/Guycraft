#pragma once
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include <glm/glm.hpp>

#include <SmartChunk.h>
#include <mutex>

class ChunkGroupContainer {
private:
	std::mutex mutex;
public:
	typedef std::unordered_map<glm::ivec2, SmartChunkGroup* > ChunkGroups;
	ChunkGroups m_container;
	void lock() { mutex.lock(); }
	void unlock() { mutex.unlock(); }
	bool exist(glm::ivec2 pos) {
		return m_container.find(pos) != m_container.end();
	}
};