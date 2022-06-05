#include <TextureAtlas.h>
#include <ResourceManager.h>
#include <stb/stb_image.h>

TextureAtlas::~TextureAtlas()
{
	glDeleteTextures(1, &m_glTextureID);
}

TextureAtlas::TextureAtlas(std::string ID, std::string pathFile)
{
	m_id = ID;
	m_pathFile = pathFile;
	reload();
}
void TextureAtlas::active(GLenum texture)
{
	glActiveTexture(texture);
	glBindTexture(GL_TEXTURE_2D, m_glTextureID);
}

void TextureAtlas::reload()
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	unsigned char* data = stbi_load(m_pathFile.c_str(), &width, &height, &channels, 0);
	if (not data) {
		printf("atlas not found \n");
		stbi_image_free(data);

		return;
	}
	if(m_glTextureID == 0) glGenTextures(1, &m_glTextureID);
	active();
	int glColorFormat = channels == 4 ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, glColorFormat, width, height,
		0, glColorFormat, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	float maxAnisotropy = 1.f;
	if (m_mipmalLevel > 0) {
		m_mipmalLevel = (m_mipmalLevel >= 5) ? 4 : m_mipmalLevel;
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, m_lodBias);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, m_mipmalLevel);
	

	m_texSize.x = width;
	m_texSize.y = height;
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureAtlas::addTextureUV(std::string texID, glm::ivec2 tile)
{
	float tileX = tile.x << 4;
	float tileY = tile.y << 4;

	TextureUV texUV;
	texUV.id = texID;
	texUV.start = { tileX, tileY };
	texUV.end = texUV.start + texUV.size;
	m_textures.emplace(texID, texUV);
}
