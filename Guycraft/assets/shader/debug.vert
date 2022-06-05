#version 330 core
layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
out vec3 vertexColor;
uniform mat4 projection;
void main()
{
    vertexColor = aNormal;
    gl_Position = projection * vec4(aVertex, 1.0);
}