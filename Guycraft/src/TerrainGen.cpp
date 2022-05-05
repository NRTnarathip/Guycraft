#include <TerrainGen.h>
#include <FastNoise/FastNoise.h>
#include <Chunk.h>

FastNoise::SmartNode<> noise = FastNoise::New<FastNoise::FractalFBm>();
FastNoise::SmartNode<> perlin = FastNoise::New<FastNoise::Perlin>();

void TerrainGen::populate(ChunkGroup* cg) {
    //set temp var
    //error
    getHeightMap(cg);

    for (auto c : cg->chunks) {
        for (unsigned char z = 0; z < Chunk::CHUNK_SIZE; z++) {
            for (unsigned char x = 0; x < Chunk::CHUNK_SIZE; x++) {
                unsigned char heightVar = cg->heightMap[x + (z << 5)];
                for (unsigned char y = 0; y < Chunk::CHUNK_SIZE; y++) {
                    c->voxels[x + (y << 5) + (z << 10)] = GetVoxelTerrain(c, heightVar, x, y, z);;
                }
            }
        }
    }
}
int TerrainGen::GetHeight(glm::vec2 pos)
{
    float nx = pos.x / Chunk::CHUNK_SIZE;
    float nz = pos.y / Chunk::CHUNK_SIZE;

    float e = perlin->GenSingle2D(nx,nz,seed);
    //convert range -1 <--> +1; to 0 <--> +1
    e = (e + 1) / 2.f;
    int height = e * 256;
    return 80;
}
void TerrainGen::getHeightMap(ChunkGroup* cg) {
    //else not found
    for (unsigned char k = 0; k < Chunk::CHUNK_SIZE; k++)
    {
        for (unsigned char i = 0; i < Chunk::CHUNK_SIZE; i++)
        {
            int posX = cg->pos.x + i;
            int posZ = cg->pos.y + k;
            cg->heightMap[i + (k<<5)] = (unsigned char)GetHeight(glm::vec2(posX,posZ));
        }
    }
}
Voxel TerrainGen::GetVoxelTerrain(Chunk*c, unsigned char heightMapVar,int x, int y, int z) {
    Voxel voxTemp;
    voxTemp.id = 0;
    voxTemp.type = 0;

    int yWorld = y + c->pos.y;

    unsigned short height = heightMapVar;//range 32*32
    //height range 0--> 255
    if (yWorld == height) {//under height or ground height
        //dirt
        voxTemp.type = 1;
    }

    else if (yWorld < height) {
        voxTemp.type = 2;//stone
    }

    return voxTemp;
}
