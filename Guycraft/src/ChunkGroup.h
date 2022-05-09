#pragma once
#include <glm/vec3.hpp> // glm::ivec3
#include <Chunk.h>
#include <mutex>
class SmartChunkGroup;

class ChunkGroup {
public:
	SmartChunkGroup* sm = nullptr;
	std::mutex mutex;
	static const unsigned int CHUNK_COUNT = 8;
	Chunk *chunks[CHUNK_COUNT];
	ChunkGroup* nearNorth = 0;
	ChunkGroup* nearSouth = 0;
	ChunkGroup* nearEast = 0;
	ChunkGroup* nearWest = 0;

	ChunkGroup(glm::ivec2 initPos);
	~ChunkGroup();
	glm::ivec2 pos;
	bool checkIsHaveVoxel(Chunk* c);
	bool needGenMeshChunk = false;
	bool inRangeRender = false;
	void clearChunk();
	void render();
	void needGenMeshNear();
};