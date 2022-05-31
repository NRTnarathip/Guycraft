#pragma once
#include <vector>
#include <iostream>
#include <BlockSystem/Block.h>

class BlockDatabase;

class BlockModel {
public:
	enum Shape : uint8_t {
		Air = -1,
		Cube = 0,
		Fluid = 1,
		Cross = 2
	};
	enum FaceDirection {
		Top, Down, Front, Back, Right,Left, Side, All
	};
	struct BlockTexture {
		FaceDirection dir;
		std::string texture;
	};
	BlockModel(blocktype_t type, uint8_t shape) {
		m_type = type;
		m_shape = (Shape)shape;
	};
	
	blocktype_t m_type;
	Shape m_shape;
	std::vector<BlockTexture> m_textures;
	void addTexture(FaceDirection dir, std::string texture) {
		m_textures.push_back({ dir, texture });
	};
}; 