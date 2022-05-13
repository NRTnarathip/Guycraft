#pragma once
#include <Chunk.h>
class GraphicSetting {
public:
	GraphicSetting() {
		fogMin = CHUNK_SIZE * 12;
		fogMax = CHUNK_SIZE * 16;
	}
	bool parserConfig() {
		return true;
	}
	int fogMin,fogMax;
	int renderDistance = 4; 
	float fov = 90.0f;
};