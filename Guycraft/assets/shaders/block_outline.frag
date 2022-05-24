#version 330 core
out vec4 fragColor;
uniform vec3 colorOutline;
void main()
{   
    fragColor = vec4(colorOutline, 1.0);
}  