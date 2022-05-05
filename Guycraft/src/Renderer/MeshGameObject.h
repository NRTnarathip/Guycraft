#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <OpenGL Class/VAO.h>
#include <OpenGL Class/VBOAndEBO.h>

class MeshGameObject {
private:
	//  render data
	VAO m_vao;
	VBO m_vbo;
	EBO m_ebo;
public:
	void setupGL();
	void uploadDataMeshToGPU();
	struct Vertex {
		Vertex() {}
		Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv) {
			this->position = pos;
			this->normal = normal;
			this->uv = uv;
		}
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
	};
	struct TextureID {
		unsigned int id;
	};
	MeshGameObject();
	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<TextureID> textures;
	void render();
};