#version 330 core
//you must convert axis by aPos.x,y,z / 16
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in float aNormal;
layout (location = 3) in float aLighting;

out float sunLightFace;
out float aoLightFace;
out float lampLightFace;

out vec2 texcoord;
out vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 textureAtlasSize;

const vec3 tbNormal[6] = vec3[6](
	vec3(0.0,1.0,0.0),//up
	vec3(0.0,-1.0,0.0),//down
	vec3(0.0,0.0,1.0),//north
	vec3(0.0,0.0,-1.0),//south
	vec3(1.0,0.0,0.0),//east
	vec3(-1.0,0.0,0.0)//west
);

void main()
{
	vec3 vertexPos = aPos / vec3(16.0, 16.0, 16.0);
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	sunLightFace = int(aLighting) & 15;
	aoLightFace = (int(aLighting) >> 8) & 3;
	lampLightFace = (int(aLighting) >> 4) & 15;

	texcoord = aUV / textureAtlasSize;
	normal = tbNormal[( int(aNormal) >> 11) & 7];
}