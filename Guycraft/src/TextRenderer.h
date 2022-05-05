#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Renderer/shaderClass.h"
#include "OpenGL Class/VAO.h"
#include "OpenGL Class/VBOAndEBO.h"
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H


class TextRenderer {
private:
	static TextRenderer* m_instance;
public:
	struct Character {
	public:
		unsigned int TextureID;  // ID handle of the glyph texture
		glm::ivec2   Size;       // Size of glyph
		glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
		unsigned int Advance;    // Offset to advance to next glyph
	};
	std::map<char, Character> Characters;
	VAO vao;
	VBO vbo;
	GLFWwindow* window = nullptr;
	TextRenderer(GLFWwindow* window);
	static TextRenderer* GetInstance() { return m_instance; }
	//load front
	int setupGL();
	void renderText(std::string text, float x, float y, glm::vec2 scale, glm::vec3 color);
};