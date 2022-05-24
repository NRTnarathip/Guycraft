#version 330 core
layout (location = 0) in vec3 aVertex;
uniform mat4 projection;
uniform mat4 view;
void main()
{
    vec4 vertexWorldPos = projection * view * vec4(aVertex, 1.0);
    gl_Position = vertexWorldPos;
}