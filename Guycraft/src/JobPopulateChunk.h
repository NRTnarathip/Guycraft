#pragma once
#include <BlockSystem/Block.h>
#include <ChunkMarco.h>
#include <glm/vec2.hpp>

struct JobPopulateChunk {
	Block blocks[CHUNK_BLOCK_ALL];
	glm::ivec2 pos;
};