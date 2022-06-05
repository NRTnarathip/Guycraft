#include "TextRenderer.h"
#include <iostream>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ResourceManager.h"
#include "GUI.h"

TextRenderer* TextRenderer::m_instance = nullptr;

TextRenderer::TextRenderer(GLFWwindow* window) {
    m_instance = this;
    this->window = window;
}
int TextRenderer::setupGL() {
    auto shaderText = ResourceManager::GetInstance()->m_shaders["text"];
    shaderText->Bind();
    // FreeType
   // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, "assets/font/arial.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            // bitshift by 6 to get value in pixels (2^6 = 64)
            face->glyph->advance.x >> 6
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    vao.genBuffer();
    vbo.genBuffer();
    vao.bind();
    vbo.bind();
    vbo.bufferData(sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    vao.unbind();
    vbo.unbind();
}
void TextRenderer::renderText(std::string text, float x, float y, glm::vec2 scale, glm::vec3 color) {
    // activate corresponding render state
    auto s = ResourceManager::GetInstance()->m_shaders["text"];
    s->Bind();
    s->SetVec3("textColor", color);
    auto model = glm::mat4(1.f);
    //model = glm::translate(model, glm::vec3(0, 0, GUI::GetInstance().getRenderZOrder()));
    s->SetMat4("model", model);
    // activate corresponding render state	
    glActiveTexture(GL_TEXTURE0);
    vao.bind();

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale.x;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale.x;

        float w = ch.Size.x * scale.x;
        float h = ch.Size.y * scale.y;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos + w, ypos + h,   1.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos,     ypos + h,   0.0f, 0.0f },
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        vbo.bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        vbo.unbind();
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += ch.Advance * scale.x; 
    }
    vao.unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
}