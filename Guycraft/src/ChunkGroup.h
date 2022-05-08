#pragma once
#include <glm/vec3.hpp> // glm::ivec3
#include <Chunk.h>
#include <mutex>
class ChunkGroup {
public:
	std::mutex mutex;
	static const unsigned int CHUNK_COUNT = 8;
	Chunk *chunks[CHUNK_COUNT];
	ChunkGroup(glm::ivec2 initPos);
	~ChunkGroup();
	glm::ivec2 pos;
	bool checkIsHaveVoxel(Chunk* c);
	bool needGenMeshChunk = false;
	bool inRangeRender = false;
	void clearChunk();
	void render();
};