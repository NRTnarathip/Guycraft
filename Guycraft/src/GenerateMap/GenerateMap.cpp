#include "GenerateMap.h"
#include <FastNoise/FastNoise.h>
#include <chrono>
#include <thread>
//auto noiseGen = FastNoise::New<FastNoise::Perlin>();

void GenerateMap::genMoiseture(GenMapData* map)
{
	
}

void GenerateMap::genHeight(GenMapData* map)
{
	float chunkSize = Chunk::CHUNK_SIZE;
	float scale = .09f;
	auto cgPos = map->cg->pos;

	for (int k = 0; k < Chunk::CHUNK_SIZE; k++) {
		int z = k + cgPos.y;
		float nz = (z / chunkSize) * scale;

		for (int i = 0; i < Chunk::CHUNK_SIZE; i++) {
			int x = i + cgPos.x;
			float nx = (x / chunkSize) * scale;

			/*float n = noiseGen->GenSingle2D(nx, nz,0);
			n = (n + 1) / 2.f;
			map->data[i + (k << BS_CH)] = n;*/
		}
	}
}

