#include <Chunk.h>
#include "ChunkMeshBuilding.h"


Chunk::Chunk(glm::ivec3 pos) {
    this->pos = pos;
}
void Chunk::clearAll() {
    mesh.clearOnGPU();
}
void Chunk::lock() {
    mutex.lock();
}
void Chunk::unlock() {
    mutex.unlock();
}
void Chunk::generateMeshChunk() {
    //clear data
    vertCount = 0;
    Voxel voxTemp;
    //usefunction get voxel out chunk
	for (unsigned char z = 0; z < CHUNK_SIZE; z++) {
		for (unsigned char y = 0; y < CHUNK_SIZE; y++) {
			for (unsigned char x = 0; x < CHUNK_SIZE; x++) {
                if (isFourceStopGenerateMesh) {
                    isFourceStopGenerateMesh = false;
                    return;
                }
                voxTemp = voxels[x + (y << 5) + (z << 10)];
                if (voxTemp.type == 0) continue; //dont gen block air
                if (x == 0 or x == CHUNK_SIZE_31 or
                    y == 0 or y == CHUNK_SIZE_31 or
                    z == 0 or z == CHUNK_SIZE_31) {
                    genMeshCube(x, y, z, voxTemp, true);
                    continue;
                }
                //else
                genMeshCube(x, y, z, voxTemp, false);
			}
		}
	}
    isNeedGenerateMesh = false;
}
unsigned char Chunk::GetVoxType(char x, char y, char z)
{
    Chunk* cNow = this;
    if (x < 0)
    {
        if (cnearWest == NULL) return 0;
        cNow = cnearWest;
        x = Chunk::CHUNK_SIZE_31;
    }
    else if (x > Chunk::CHUNK_SIZE_31)
    {
        if (cnearEast == NULL) return 0;
        cNow = cnearEast;
        x = 0;
    }

    if (y < 0)
    {
        if (cNow->cnearDown == NULL) return 0;
        cNow = cNow->cnearDown;
        y = Chunk::CHUNK_SIZE_31;
    }
    else if (y > Chunk::CHUNK_SIZE_31)
    {
        if (cNow->cnearUp == NULL) return 0;
        cNow = cNow->cnearUp;
        y = 0;
    }

    if (z < 0)
    {
        if (cNow->cnearSouth == NULL) return 0;
        cNow = cNow->cnearSouth;
        z = Chunk::CHUNK_SIZE_31;
    }
    else if (z > Chunk::CHUNK_SIZE_31)
    {
        if (cNow->cnearNorth == NULL) return 0;
        cNow = cNow->cnearNorth;
        z = 0;
    }

    return cNow->voxels[x + (y << 5) + (z << 10)].type;
}
void Chunk::GetVoxSurround(unsigned char (&args)[27], unsigned char x, unsigned char y, unsigned char z,bool useFuncGetVoxOutChunk)
{
    int index = 0;
    //optimize
    //use function check next voxel of this chunk
    if (useFuncGetVoxOutChunk) {

        for (int j = y - 1; j <= y + 1; j++)
            for (int k = z - 1; k <= z + 1; k++)
                for (int i = x - 1; i <= x + 1; i++)
                {
                    // voxSideSurr 0 == non block
                    //else if 1 == is found block
                    if (GetVoxType(i, j, k) > 0) args[index] = 1;
                    else args[index] = 0;

                    index++;
                }
        return;
    }
    //get voxel in chunk
    for (int j = y - 1; j <= y + 1; j++)
        for (int k = z - 1; k <= z + 1; k++)
            for (int i = x - 1; i <= x + 1; i++)
            {
                auto voxType = voxels[i + (j << 5) + (k << 10)].type;
                if(voxType > 0) args[index] = 1;
                else args[index] = 0;

                index++;
            }
}
void Chunk::GetAO(unsigned char (&refAO)[4], unsigned char dir, unsigned char (&voxSurr)[27]) {
    if (dir == 0)//Face up
    {
        refAO[0] = CalculateAO(voxSurr[19], voxSurr[21], voxSurr[18]);
        refAO[1] = CalculateAO(voxSurr[21], voxSurr[25], voxSurr[24]);
        refAO[2] = CalculateAO(voxSurr[25], voxSurr[23], voxSurr[26]);
        refAO[3] = CalculateAO(voxSurr[23], voxSurr[19], voxSurr[20]);
        return;
    }
    else if (dir == 1)//down
    {
        refAO[0] = CalculateAO(voxSurr[7], voxSurr[3], voxSurr[6]);
        refAO[1] = CalculateAO(voxSurr[3], voxSurr[1], voxSurr[0]);
        refAO[2] = CalculateAO(voxSurr[1], voxSurr[5], voxSurr[2]);
        refAO[3] = CalculateAO(voxSurr[5], voxSurr[7], voxSurr[8]);
        return;
    }
    else if (dir == 2)//north
    {
        refAO[0] = CalculateAO(voxSurr[7], voxSurr[17], voxSurr[8]);
        refAO[1] = CalculateAO(voxSurr[17], voxSurr[25], voxSurr[26]);
        refAO[2] = CalculateAO(voxSurr[25], voxSurr[15], voxSurr[24]);
        refAO[3] = CalculateAO(voxSurr[15], voxSurr[7], voxSurr[6]);
        return;
    }
    else if (dir == 3)//south
    {
        refAO[0] = CalculateAO(voxSurr[1], voxSurr[9], voxSurr[0]);
        refAO[1] = CalculateAO(voxSurr[9], voxSurr[19], voxSurr[18]);
        refAO[2] = CalculateAO(voxSurr[19], voxSurr[11], voxSurr[20]);
        refAO[3] = CalculateAO(voxSurr[11], voxSurr[1], voxSurr[2]);
        return;
    }
    else if (dir == 4)//east
    {
        refAO[0] = CalculateAO(voxSurr[5], voxSurr[11], voxSurr[2]);
        refAO[1] = CalculateAO(voxSurr[11], voxSurr[23], voxSurr[20]);
        refAO[2] = CalculateAO(voxSurr[23], voxSurr[17], voxSurr[26]);
        refAO[3] = CalculateAO(voxSurr[17], voxSurr[5], voxSurr[8]);
        return;
    }
    //west
    refAO[0] = CalculateAO(voxSurr[3], voxSurr[15], voxSurr[6]);
    refAO[1] = CalculateAO(voxSurr[15], voxSurr[21], voxSurr[24]);
    refAO[2] = CalculateAO(voxSurr[21], voxSurr[9], voxSurr[18]);
    refAO[3] = CalculateAO(voxSurr[9], voxSurr[3], voxSurr[0]);
    return;
}
unsigned char Chunk::CalculateAO(unsigned char side1, unsigned char side2, unsigned char corner) {
    if (side1 == 1 && side2 == 1)
    {
        return 3;
    }
    return (unsigned char)(side1 + side2 + corner);
}
void Chunk::GetVertLightMaping(unsigned char(&ref)[4], unsigned char dir, unsigned char (&lightSurr)[27]) {
    if (dir == 0) {//up
        ref[0] = lightSurr[22];
        ref[1] = lightSurr[22];
        ref[2] = lightSurr[22];
        ref[3] = lightSurr[22];
    }
    else if (dir == 1) {//down
        ref[0] = lightSurr[4];
        ref[1] = lightSurr[4];
        ref[2] = lightSurr[4];
        ref[3] = lightSurr[4];
    }
    else if (dir == 2) {//north
        ref[0] = lightSurr[16];
        ref[1] = lightSurr[16];
        ref[2] = lightSurr[16];
        ref[3] = lightSurr[16];
    }
    else if (dir == 3) {//south
        ref[0] = lightSurr[10];
        ref[1] = lightSurr[10];
        ref[2] = lightSurr[10];
        ref[3] = lightSurr[10];
    }
    else if (dir == 4) {//east
        ref[0] = lightSurr[14];
        ref[1] = lightSurr[14];
        ref[2] = lightSurr[14];
        ref[3] = lightSurr[14];
    }
    //west
    ref[0] = lightSurr[12];
    ref[1] = lightSurr[12];
    ref[2] = lightSurr[12];
    ref[3] = lightSurr[12];
}
void Chunk::MakeQuadFace(Voxel vox, unsigned char directFace, unsigned char (&voxSurr)[27], unsigned char (&lightSurr)[27]) {
    unsigned int vertCountMinus_1 = vertCount - 1;//index 3
    unsigned int vertCountMinus_3 = vertCount - 3;//index 1

    mesh.triangles.push_back(vertCount - 4);//0
    mesh.triangles.push_back(vertCountMinus_3);//1
    mesh.triangles.push_back(vertCountMinus_1);//3

    mesh.triangles.push_back(vertCount - 2);//2
    mesh.triangles.push_back(vertCountMinus_1);//3
    mesh.triangles.push_back(vertCountMinus_3);//1

    //ao use 2bit 0->3;
    //2bit *4 = 8bit or 1 byte;
    unsigned char aos[4];
    //light map between sun and lamp light
    unsigned char lightMaping[4];
    GetAO(aos, directFace, voxSurr);
    //flip ao if

    GetVertLightMaping(lightMaping, directFace, lightSurr);

    //lightmap
    //set vertex from vertCount - 4 to vertcount - 1;

    for (int i = 0; i < 4; i++) {
        MeshChunk::Vertex& vert = mesh.vertexs[ vertCount - (4 - i) ];// -4 --> -1
        vert.SetUVIndex(i);
        vert.SetAO(aos[i]);
    }
}
void Chunk::genMeshCube(char x, char y, char z, Voxel vox,bool useFuncGetVoxOutChunk) {
    // meaning we should check near chunk
    if (useFuncGetVoxOutChunk) {

    }
    //var temp;
    //Precalculate var
    //var temp
    char xx = x + 1;
    char yy = y + 1;
    char zz = z + 1;
    unsigned char voxSurr[27];
    unsigned char lightSurr[27];//lighting surround
    MeshChunk::Vertex vert;
    
    GetVoxSurround(voxSurr, x, y, z, useFuncGetVoxOutChunk);
    //GetLightingSurround(lightSurr,x,y,z);
    vert.SetUVTile(vox.id, vox.type, 16);
    int disbleFace = 6;
    if (FaceIsVisableUp(x, yy, z) and disbleFace != 0)
    {
        vert.SetNormal(0);
        vert.SetPos(x, yy, z);
        mesh.vertexs.push_back(vert);//0
        vert.SetPos(x, yy, zz);
        mesh.vertexs.push_back(vert);//1
        vert.SetPos(xx, yy, zz);
        mesh.vertexs.push_back(vert);//2
        vert.SetPos(xx, yy, z);
        mesh.vertexs.push_back(vert);//3
        vertCount += 4;
        MakeQuadFace(vox, 0, voxSurr, lightSurr);
    }
    if (FaceIsVisableDown(x,y - 1, z))
    {
        vert.SetNormal(1);
        vert.SetPos(x, y, zz);
        mesh.vertexs.push_back(vert);

        vert.SetPos(x, y, z);
        mesh.vertexs.push_back(vert);

        vert.SetPos(xx, y, z);
        mesh.vertexs.push_back(vert);

        vert.SetPos(xx, y, zz);
        mesh.vertexs.push_back(vert);
        vertCount += 4;
        MakeQuadFace(vox, 1, voxSurr, lightSurr);
    }
    if (FaceIsVisableNorth(x, y, zz))
    {
        vert.SetNormal(2);
        vert.SetPos(xx, y, zz);
        mesh.vertexs.push_back(vert);

        vert.SetPos(xx, yy, zz);
        mesh.vertexs.push_back(vert);

        vert.SetPos(x, yy, zz);
        mesh.vertexs.push_back(vert);

        vert.SetPos(x, y, zz);
        mesh.vertexs.push_back(vert);
        vertCount += 4;
        MakeQuadFace(vox, 2, voxSurr, lightSurr);
    }
    if (FaceIsVisableSouth(x, y, z - 1))
    {
        vert.SetNormal(3);
        vert.SetPos(x, y, z);
        mesh.vertexs.push_back(vert);

        vert.SetPos(x, yy, z);
        mesh.vertexs.push_back(vert);

        vert.SetPos(xx, yy, z);
        mesh.vertexs.push_back(vert);

        vert.SetPos(xx, y, z);
        mesh.vertexs.push_back(vert);
        vertCount += 4;

        MakeQuadFace(vox, 3, voxSurr, lightSurr);
    }
    if (FaceIsVisableEast(xx, y, z))
    {
        vert.SetNormal(4);
        vert.SetPos(xx, y, z);
        mesh.vertexs.push_back(vert);

        vert.SetPos(xx, yy, z);
        mesh.vertexs.push_back(vert);

        vert.SetPos(xx, yy, zz);
        mesh.vertexs.push_back(vert);

        vert.SetPos(xx, y, zz);
        mesh.vertexs.push_back(vert);
        vertCount += 4;

        MakeQuadFace(vox, 4, voxSurr, lightSurr);
    }
    if (FaceIsVisableWest(x - 1, y, z))
    {
        vert.SetNormal(5);
        vert.SetPos(x, y, zz);
        mesh.vertexs.push_back(vert);

        vert.SetPos(x, yy, zz);
        mesh.vertexs.push_back(vert);

        vert.SetPos(x, yy, z);
        mesh.vertexs.push_back(vert);

        vert.SetPos(x, y, z);
        mesh.vertexs.push_back(vert);
        vertCount += 4;

        MakeQuadFace(vox, 5, voxSurr, lightSurr);
    }
}
//for check next block if type is air or transparency we should gen now mesh
bool Chunk::FaceIsVisableUp(char i, char j, char k)
{
    if (j > CHUNK_SIZE_31)
    {
        //render if block is limit height of chunk
        if (pos.y == 240)
            return true;
        else if (cnearUp == NULL)
            return false;
        
        return cnearUp->voxels[i + (k << 10)].type == 0;
    }
    return voxels[i + (j << 5) + (k << 10)].type == 0;

}
bool Chunk::FaceIsVisableDown(char i, char j, char k)
{
    if (j < 0)
    {
        //render face if chunk is hiehgt is zero
        if (pos.y == 0)
            return true;
        else if (cnearDown == NULL)
            return false;
        return cnearDown->voxels[i + (CHUNK_SIZE_31 << 5) + (k << 10)].type == 0;
    }
    return voxels[i + (j << 5) + (k << 10)].type == 0;
}
bool Chunk::FaceIsVisableNorth(char i, char j, char k)
{
    if (k > CHUNK_SIZE_31)
    {
        if (cnearNorth == NULL)
            return false;
        return cnearNorth->voxels[i + (j << 5)].type == 0;
    }
    return voxels[i + (j << 5) + (k << 10)].type == 0;
}
bool Chunk::FaceIsVisableSouth(char i, char j, char k)
{
    if (k < 0)
    {
        if (cnearSouth == NULL)
            return false;

        return cnearSouth->voxels[i + (j << 5) + (CHUNK_SIZE_31 << 10)].type == 0;
    }
    return voxels[i + (j << 5) + (k << 10)].type == 0;
}
bool Chunk::FaceIsVisableEast(char i, char j, char k)
{
    if (i > CHUNK_SIZE_31)
    {
        if (cnearEast == NULL)
            return false;
        return cnearEast->voxels[(j << 5) + (k << 10)].type == 0;
    }
    return voxels[i + (j << 5) + (k << 10)].type == 0;
}
bool Chunk::FaceIsVisableWest(char i, char j, char k)
{
    if (i < 0)
    {
        if (cnearWest == NULL)
            return false;
        return cnearWest->voxels[CHUNK_SIZE_31 + (j << 5) + (k << 10)].type == 0;
    }
    return voxels[i + (j << 5) + (k << 10)].type == 0;
}
//get set lightmap
unsigned char Chunk::GetLightExtra(int x, int y, int z)
{
    Chunk* cNow = this;
    if (x < 0)
    {
        if (cnearWest == NULL) return 0;
        cNow = cnearWest;
        x = 15;
    }
    else if (x > 15)
    {
        if (cnearEast == NULL) return 0;
        cNow = cnearEast;
        x = 0;
    }
    if (y < 0)
    {
        if (cNow->cnearDown == NULL) return 0;
        cNow = cNow->cnearDown;
        y = 15;
    }
    else if (y > 15)
    {
        if (cNow->cnearUp == NULL) return 0;
        cNow = cNow->cnearUp;
        y = 0;
    }
    if (z < 0)
    {
        if (cNow->cnearUp == NULL) return 0;
        cNow = cNow->cnearUp;
        z = 15;
    }
    else if (z > 15)
    {
        if (cNow->cnearUp == NULL) return 0;
        cNow = cNow->cnearUp;
        z = 0;
    }
    return cNow->lightMap[x + (y << 4) + (z << 8)];
}
void Chunk::GetLightingSurround(unsigned char (&refLightMap)[27],int _x, int _y, int _z)
{
    int ind = 0;
    for (int y = (int)(_y - 1); y <= _y + 1; y++)
    {
        for (int z = (int)(_z - 1); z <= _z + 1; z++)
        {
            for (int x = (int)(_x - 1); x <= _x + 1; x++)
            {
                int lit = GetLightExtra(x, y, z);
                refLightMap[ind] = lit;
                ind++;
            }
        }
    }
}
unsigned char Chunk::GetSunLight(unsigned short acc) {
    return lightMap[acc] & 15;
}
unsigned char Chunk::GetLampLight(unsigned short acc) {
    return (lightMap[acc] >> 4 ) & 15;
}
void Chunk::SetLampLight(unsigned short acc, unsigned char val) {
    //mask 240;//bit 1111 0000
    lightMap[acc] = (lightMap[acc] & 0xF0 )| (val);
}
void Chunk::SetSunLight(unsigned short acc, unsigned char val) {
    //mask 15; bit 0000 1111
    lightMap[acc] = (lightMap[acc]& 0x0F) | (val << 4);
}