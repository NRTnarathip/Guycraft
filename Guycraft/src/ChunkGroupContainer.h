#pragma once
#include <unordered_map>
#include <glm/glm.hpp>
#include <SmartChunk.h>

class ChunkGroupContainer {
private:
	std::mutex mutex;
public:
	typedef std::unordered_map<glm::ivec2, SmartChunkGroup* > ChunkGroups;
	ChunkGroups m_container;
	void lock() { mutex.lock(); }
	void unlock() { mutex.unlock(); }
};