#pragma once
#include <ChunkGroup.h>
//for data nosie map such as height, heat, moiseture
struct GenMapData
{
	ChunkGroup* cg;
	float data[Chunk::CHUNK_SIZE_SQUARED] = {};
};
class GenerateMap {
public:
	int seed = 0;
	void genMoiseture(GenMapData* map);
	void genHeight(GenMapData* map);
};