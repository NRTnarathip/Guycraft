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
	ChunkGroup* nearNorth = nullptr;
	ChunkGroup* nearSouth = nullptr;
	ChunkGroup* nearEast = nullptr;
	ChunkGroup* nearWest = nullptr;

	ChunkGroup(glm::ivec2 initPos);
	~ChunkGroup();
	glm::ivec2 pos;
	bool isFourceUnload = false;
	void fourceUnload();
	bool checkIsHaveVoxel(Chunk* c);
	bool needGenMeshChunk = false;
	void clearChunk();
	void render();
	void needGenMeshNear();
	void linkNeighborChunk(ChunkGroup* cgNeighbor[4]);
	void unlinkNeighborChunk();
};