#pragma once

#include <Voxel.h>
#include <MeshChunk.h>
#include <Renderer/shaderClass.h>
#include <glm/vec3.hpp>
#include <mutex>
#define Bitsift_ChunkSize << 5
#define Bitsift_ChunkSizeSquared << 10
#define BS_CH 5 //bitsift to Chunk Size
#define BS_CH2 10 //bitshift to Chunk Size Squared

//none safe thread
class Chunk {
public:
	std::mutex mutex;
	static const int CHUNK_SIZE = 32;
	static const int CHUNK_SIZE_SQUARED = CHUNK_SIZE * CHUNK_SIZE;
	static const int CHUNK_SIZE_BLOCK = CHUNK_SIZE_SQUARED * CHUNK_SIZE;
	static const int CHUNK_SIZE_31 = 31;
	static const int CHUNK_SIZE_MAX_INDEX = CHUNK_SIZE - 1;
	static const int MAX_HEIGHT = 240; // index == 7, index 8 is out limit of array
	Chunk(glm::ivec3 pos);
	//bitwise light lamp and sun;
	unsigned char lightMap[CHUNK_SIZE_BLOCK];
	glm::ivec3 pos;
	Voxel voxels[CHUNK_SIZE_BLOCK];
	MeshChunk mesh;
	void clearAll();//reset all for poolling
	void generateMeshChunk();
	void genMeshCube(char x, char y, char z, Voxel vox,
		bool useFuncitonGetVoxelOutChunk);
	bool isNeedDelete = false;
	bool isGenerateMesh = false;
	bool isNeedRegenerateMesh = false;
	bool isNeedGenerateMesh = false;
	bool isFourceUnload = false;

	//for threading
	void lock();
	void unlock();
	void fourceUnload();

	Chunk* cnearUp = NULL;
	Chunk* cnearDown = NULL;
	Chunk* cnearNorth = NULL;
	Chunk* cnearSouth = NULL;
	Chunk* cnearEast = NULL;
	Chunk* cnearWest =NULL;

	bool FaceIsVisableUp(char i, char j, char k);
	bool FaceIsVisableDown(char i, char j, char k);
	bool FaceIsVisableNorth(char i, char j, char k);
	bool FaceIsVisableSouth(char i, char j, char k);
	bool FaceIsVisableEast(char i, char j, char k);
	bool FaceIsVisableWest(char i, char j, char k);
	//voxel helper
	void GetVoxSurround(unsigned char(&args)[27], unsigned char x, unsigned char y, unsigned char z,
		bool useFuncitonGetVoxelOutChunk);
	unsigned char GetVoxType(char x, char y, char z);
	void MakeQuadFace(Voxel voxel, unsigned char directFace, unsigned char (&voxSurr)[27], unsigned char (&lightSurr)[27]);
	//color face
	void GetAO(unsigned char(&refAO)[4], unsigned char directFace, unsigned char(&voxSurr)[27]);
	void GetVertLightMaping(unsigned char(&ref)[4], unsigned char dir, unsigned char(&voxSurr)[27]);
	static unsigned char CalculateAO(unsigned char side1, unsigned char side2, unsigned char corner);
	unsigned char GetLightExtra(int x, int y, int z);
	unsigned char GetSunLight(unsigned short acc);
	unsigned char GetLampLight(unsigned short acc);
	void SetSunLight(unsigned short acc, unsigned char val);
	void SetLampLight(unsigned short acc, unsigned char val);

	void GetLightingSurround(unsigned char(&refLightMap)[27], int _x, int _y, int _z);
	//add uv
	static const unsigned char tileCountRow = 16;
	static const float tileSize;
private:
	//for method generate mesh
	int vertCount = 0;//mesh vertex count
};