#version 330 core
layout (location = 0) in vec3 aVertex;
uniform mat4 projection;
uniform mat4 view;
out vec4 vertexPos;
void main()
{
    vertexPos = projection * view * vec4(aVertex, 1.0);;
    gl_Position = vertexPos;
}