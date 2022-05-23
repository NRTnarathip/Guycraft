#version 330 core
out vec4 fragColor;
in vec4 vertexPos;
void main()
{   
    vec3 normal = vec3(0,1,0);
    vec3 color = vec3(1.0);
    float colOutline = exp(dot(vec3(0, 0, 1), normal));

    fragColor = vec4(color * colOutline, 1.0);
}  