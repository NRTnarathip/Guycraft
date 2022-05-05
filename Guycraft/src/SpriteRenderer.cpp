#include "SpriteRenderer.h"
void SpriteRenderer::draw() {
    sprite->refTexture->Activate(GL_TEXTURE0);
    sprite->vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    sprite->vao.unbind();
}
