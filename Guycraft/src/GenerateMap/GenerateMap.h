#pragma once
#include <Chunk.h>
//for data nosie map such as height, heat, moiseture
struct GenMapData
{
	Chunk* cg;
	float data[CHUNK_SIZE_SQUARED] = {};
};
class GenerateMap {
public:
	int seed = 0;
	void genMoiseture(GenMapData* map);
	void genHeight(GenMapData* map);
};