#pragma once
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>
struct TextureUV {
	std::string id;
	//unit pixel
	glm::vec2 size{16, 16};
	//unit pixel
	glm::vec2 start;
	//unit pixel
	glm::vec2 end;
};
class TextureAtlas {
public:
	~TextureAtlas();
	TextureAtlas(std::string ID, std::string pathFile);
	std::unordered_map<std::string, TextureUV> m_textures;
	int m_mipmalLevel = 4;
	float m_lodBias = -4.f;

	void active(GLenum texture = GL_TEXTURE0);
	std::string getID() { return m_id; }
	//image size normal at 16*16
	glm::vec2 getSize() {
		return m_texSize;
	};
	
	void reload();
	void addTextureUV(std::string texID, glm::ivec2 tile );
private:
	//unit pixel
	glm::vec2 m_texSize;
	std::string m_id;
	
	GLuint m_glTextureID = 0;
	std::string m_pathFile;
};