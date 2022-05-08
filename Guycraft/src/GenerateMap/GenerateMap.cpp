#include "GenerateMap.h"
#include <FastNoise/FastNoise.h>
FastNoise::SmartNode<> perlin = FastNoise::New<FastNoise::Perlin>();

void GenerateMap::genMoiseture(GenMapData* map)
{
	
}

void GenerateMap::genHeight(GenMapData* map)
{
	float chunkSize = Chunk::CHUNK_SIZE;
	float scale = .2f;

	for(int k = 0; k < Chunk::CHUNK_SIZE; k++) {
		for (int i = 0; i < Chunk::CHUNK_SIZE; i++) {
			int x = map->cg->pos.x + i;
			int z = map->cg->pos.y + k;
			float nx = (x / chunkSize) * scale;
			float nz = (z / chunkSize) * scale;
			float n = perlin->GenSingle2D(nx, nz, seed);
			n = (n + 1) / 2.f;
			map->data[i + (k << 5)] = n;
		}
	}
}

