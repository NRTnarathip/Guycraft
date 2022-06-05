#pragma once
#include <vector>
#include <iostream>
#include <BlockSystem/Block.h>
#include <glm/glm.hpp>
#include <TextureAtlas.h>

enum Shape {
	Invisible = -1,
	Cube,
	Cross,
	Tourch,
	Water,
	Stair
};
enum Material {
	Opaque,
	Alpha,
	Blend,
	DoubleSide //none backface culling
};
struct BlockUV {
	glm::vec2 start;
	glm::vec2 end;
};
class BlockModel {
public:
	enum FaceDirection {
		Top, Buttom, Front, Back, Right,Left, Side, All
	};
	struct BlockTexture {
		FaceDirection dir;
		BlockUV uv;
		std::string textureID;
	};
	BlockModel(blocktype_t type, Shape shape) {
		m_type = type;
		m_shape = shape;
		m_material = Material::Opaque;
	};
	
	static void GetBlockTextureUV(BlockModel *model,TextureUV (&uvs)[6]);
	TextureAtlas* m_atlas = nullptr;
	blocktype_t m_type;
	Shape m_shape;
	Material m_material;

	std::vector<BlockTexture> m_textures;
	void addTexture(FaceDirection dir, std::string textureID);
}; 