#pragma once
#include <Chunk.h>

class ChunkHelper {
public:
	Chunk* up;
	Chunk* down;
	Chunk* north;
	Chunk* south;
	Chunk* east;
	Chunk* west;
};