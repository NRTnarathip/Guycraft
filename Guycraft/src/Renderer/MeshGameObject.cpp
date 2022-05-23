#include "Renderer/MeshGameObject.h"
#include <glad/glad.h>
#include <cstddef>

MeshGameObject::MeshGameObject() {

}
void MeshGameObject::uploadDataMeshToGPU() {
    if (vertices.size() == 0) {
        return;
    }

    m_vao.bind();
    m_vbo.bind();
    m_vbo.bufferData(vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    m_ebo.bind();
    m_ebo.bufferData(indices.size() * sizeof(unsigned int),
        &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    m_vao.unbind();
}
void MeshGameObject::setupGL() {
    m_vao.genBuffer();
    m_vbo.genBuffer();
    m_ebo.genBuffer();
}
void MeshGameObject::render() {
    // draw mesh
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    m_vao.unbind();
}