#pragma once
#include <vector>
#include <iostream>

class BlockDatabase;

class BlockModel {
public:
	BlockModel(uint16_t voxelType) {
		this->voxelType = voxelType;
	}
	enum FaceDirection {
		Top,Down,Front,Back,Left,Right,Side,All
	};
	struct BlockTexture {
		FaceDirection dir;
		std::string texture;
	};
	uint8_t voxelType;
	std::vector<BlockTexture> m_textures;
	void addTexture(FaceDirection dir, std::string texture) {
		m_textures.push_back({ dir, texture });
	};
}; 