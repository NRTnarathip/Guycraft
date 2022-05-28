#version 330 core
//you must convert axis by aPos.x,y,z / 16
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aUVTileAndNormal;
layout (location = 2) in float aLighting;

out float lightFace;
out float ao;
out vec2 texcoord;
out vec3 normal;

const float tileSize = 0.0625;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
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
	int x = int(val) & 15;
	int y = (int(val) >> 4) & 15;

	float tileX = x * tileSize;
	float tileY = y * tileSize;

	int vertIndex = (int(val) >> 8) & 3;

	vec2 uv = vec2(tileX,tileY);
	uv += tbUV[vertIndex] * tileSize;
	return uv;
};

void main()
{
	vec3 vertexPos = aPos / vec3(16.0, 16.0, 16.0);
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	ao = (int(aLighting) >> 8) & 3;
	lightFace = int(aLighting); //temp

	texcoord = toTextureCood(aUVTileAndNormal);
	normal = tbNormal[( int(aUVTileAndNormal) >> 11) & 7];
}