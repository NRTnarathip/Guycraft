#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <assert.h>
#include <algorithm>

Texture::Texture(const char* path, bool mipmap, int wrapMode, int filter)
{
	// Initialize gpu texture
	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	// Load image pixels
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width_, &height_, &channels_, 0);
	printf("stbi_load %s channel: %d\n", path,channels_);
	if (data)
	{
		// Send pixel data to texture
		int glAlpha = channels_ == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, glAlpha, width_, height_, 0, glAlpha, GL_UNSIGNED_BYTE, data);

		// Generate mipmaps
		if (mipmap) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	else
	{
		assert(false); //error
	}

	// Free cpu pixel data
	stbi_image_free(data);
}

int Texture::GetWidth() const
{
	return width_;
}

int Texture::GetHeight() const
{
	return height_;
}

int Texture::GetChannelCount() const
{
	return channels_;
}

void Texture::Activate(GLenum texture)
{
	glActiveTexture(texture);
	glBindTexture(GL_TEXTURE_2D, id_);
}