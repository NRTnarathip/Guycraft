#include <TerrainGen.h>
#include <Chunk.h>
#include <GenerateMap/GenerateMap.h>

void TerrainGen::populate(ChunkGroup* cg) {
    GenMapData hm = {cg};
    genmap.genHeight(&hm);
    for (auto c : cg->chunks) {
        
        //place base biome block
        for (unsigned char z = 0; z < Chunk::CHUNK_SIZE; z++) {
            for (unsigned char x = 0; x < Chunk::CHUNK_SIZE; x++) {
                int accDataMap = x + (z << BS_CH);
                float noiseData[3] = { hm.data[accDataMap],0,0 };
                for (unsigned char y = 0; y < Chunk::CHUNK_SIZE; y++) {
                    c->voxels[x + (y << BS_CH) + (z << BS_CH2)] = GetVoxelTerrain(c, noiseData, y);
                }
            }
        }
        //place ore
    }
}
Voxel TerrainGen::GetVoxelTerrain(Chunk*c, float genMapData[3], int localY) {
    Voxel voxTemp;
    voxTemp.id = 0;
    voxTemp.type = 0;

    auto y = localY + c->pos.y;
    int height =  (int)(genMapData[0] * 255);
    if (y == height) {//under height or ground height
        //dirt
        voxTemp.type = 1;
    }

    else if (y < height) {
        voxTemp.type = 2;//stone
    }

    return voxTemp;
}
