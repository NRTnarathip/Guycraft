#include "SpriteRenderer.h"
void SpriteRenderer::initRenderer()
{
    vao.setupGL();
    vbo.setupGL();
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
    };

    vao.bind();
    vbo.bind();

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void SpriteRenderer::setSprite(Sprite* sprite)
{
    m_sprite = sprite;
}
void SpriteRenderer::draw() {
    m_sprite->refTexture->Activate(GL_TEXTURE0);
    vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    vao.unbind();
}
