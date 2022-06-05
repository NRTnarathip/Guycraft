#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

class Texture
{
public:
	Texture(const char* path, int wrapMode = GL_REPEAT, int filter = GL_NEAREST);
	int GetWidth() const;
	int GetHeight() const;
	int GetChannelCount() const;
	glm::ivec2 getSize() const {
		return glm::ivec2(width_, height_);
	};
	// Use this texture to draw
	void Activate(GLenum texture = 0);
private:
	unsigned id_;
	int width_;
	int height_;
	int channels_;
};