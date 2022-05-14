#version 330 core
//you must convert axis by aPos.x,y,z / 16
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aUVTileAndNormal;
layout (location = 2) in float aLighting;

out float sunLampLight;
out float ao;
out vec2 texcoord;
out vec3 normal;

const float tileSize = 0.0625;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float time;

float waveHeight = 0.0625;


//Warning!!!! u invert axis X
const vec2 tbUV[4] = vec2[4](
	vec2(0.0,0.0),//0
	vec2(0.0,1.0),//1
	vec2(1.0,1.0),//2
	vec2(1.0,0.0)//3
);

const vec3 tbNormal[6] = vec3[6](
	vec3(0.0,1.0,0.0),//up
	vec3(0.0,-1.0,0.0),//down
	vec3(0.0,0.0,1.0),//north
	vec3(0.0,0.0,-1.0),//south
	vec3(1.0,0.0,0.0),//east
	vec3(-1.0,0.0,0.0)//west
);
vec2 toTextureCood(float val) {
	int x = int(val) & 63;
	int y = (int(val) >> 6) & 63;

	float tileX = x * tileSize;
	float tileY = y * tileSize;

	int uvIndex = (int(val) >> 12) & 3;

	vec2 uv = vec2(tileX,tileY);
	uv * tileSize;
	uv += tbUV[uvIndex] * tileSize;
	return uv;
};

void main()
{
	vec3 vertexPos = aPos / vec3(16.0, 16.0, 16.0);
	float waveX = sin(vertexPos.x + time) * waveHeight;
	float waveZ = cos(vertexPos.z + time) * waveHeight;
	vertexPos.y = vertexPos.y + ((waveX + waveZ) / 2.0);

	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	ao = (int(aLighting) >> 8) & 3;
	sunLampLight = 0; //temp

	texcoord = toTextureCood(aUVTileAndNormal);
	normal = tbNormal[( int(aUVTileAndNormal) >> 12) & 7];
}