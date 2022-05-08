#include <TerrainGen.h>
#include <Chunk.h>
#include <GenerateMap/GenerateMap.h>
#include <FastNoise/FastNoise.h>

void TerrainGen::populate(ChunkGroup* cg) {
    float heightMap[Chunk::CHUNK_SIZE_SQUARED] = {};
    float moisetureMap[Chunk::CHUNK_SIZE_SQUARED] = {};
    float heatMap[Chunk::CHUNK_SIZE_SQUARED] = {};


    float chunkSize = Chunk::CHUNK_SIZE;
    float scale = .18f;

    auto noise = FastNoise::New<FastNoise::Perlin>();

    auto cgPos = cg->pos;
    //gen all noise map
    for (int k = 0; k < Chunk::CHUNK_SIZE; k++) {
        int z = k + cgPos.y;
        float nz = (z / chunkSize) * scale;

        for (int i = 0; i < Chunk::CHUNK_SIZE; i++) {
            int x = i + cgPos.x;
            float nx = (x / chunkSize) * scale;

            //gen height map
            float n = noise->GenSingle2D(nx, nz, 20);
            n = (n + 1) / 2.f;

            heightMap[i + (k << BS_CH)] = n;
        }
    }
    //place biome block
    for (auto c : cg->chunks) {
        for (unsigned char z = 0; z < Chunk::CHUNK_SIZE; z++) {
            for (unsigned char x = 0; x < Chunk::CHUNK_SIZE; x++) {
                int accDataMap = x + (z << BS_CH);
                float noiseData[3] = { heightMap[accDataMap],0,0 };
                for (unsigned char y = 0; y < Chunk::CHUNK_SIZE; y++) {
                    c->voxels[x + (y << BS_CH) + (z << BS_CH2)] = GetVoxelTerrain(c, noiseData, y);
                }
            }
        }
    }
}
Voxel TerrainGen::GetVoxelTerrain(Chunk*c, float noiseData[3], int localY) {
    Voxel voxTemp = { 0, 0};

    auto y = localY + c->pos.y;
    int height =  (int)(noiseData[0] * 255);

    if (y == height) {
        //dirt
        voxTemp.type = 1;
    }

    else if (y < height) {
        voxTemp.type = 2;//stone
    }

    return voxTemp;
}
