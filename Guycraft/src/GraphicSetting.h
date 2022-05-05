#pragma once
#include <Chunk.h>
class GraphicSetting {
public:
	GraphicSetting() {
		fogMin = Chunk::CHUNK_SIZE * 12;
		fogMax = Chunk::CHUNK_SIZE * 16;
	}
	bool parserConfig() {
		return true;
	}
	int fogMin,fogMax;
	int renderDistance = 4; 
	float fov = 90.0f;
};