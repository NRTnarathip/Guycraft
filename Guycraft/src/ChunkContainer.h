#pragma once
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include <glm/glm.hpp>
#include <Chunk.h>
#include <mutex>

class ChunkContainer {
private:
	std::mutex mutex;
public:
	typedef std::unordered_map<glm::ivec2, Chunk* > ChunkMap;
	ChunkMap m_container;
	void lock() { mutex.lock(); }
	void unlock() { mutex.unlock(); }
	bool exist(glm::ivec2 pos) {
		return m_container.find(pos) != m_container.end();
	}
	void erase(glm::ivec2 pos) {
		m_container.erase(pos);
	}
};