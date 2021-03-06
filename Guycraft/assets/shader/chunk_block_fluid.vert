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

uniform float time;

int waveHeightPixel = 2;
float waveSpeed = 0.5;
#define PI 3.14159265
float random(vec2 c){
	return fract(sin(dot(c.xy ,vec2(12.9898, 78.233))) * 43758.5453);
}
float noise (vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}
float noiseWave(float x, float z) {
	float offs = time * waveSpeed;
	vec2 pos = vec2(x + offs, z + offs);
	return noise(pos);
}

void main()
{
	vec3 vertexLocal = aPos / vec3(16.0, 16.0, 16.0);
	vec3 vertexWorldPos = vertexLocal;
	vec4 chunkPos = model[3];
	vertexWorldPos += chunkPos.xyz;
	float waveHeight = waveHeightPixel / 16.0;
	float nw = noiseWave(vertexWorldPos.x, vertexWorldPos.z);
	float waveY = (nw * waveHeight);
	vertexLocal.y += waveY;

	gl_Position = projection * view * model * vec4(vertexLocal, 1.0);
	sunLightFace = int(aLighting) & 15;
	aoLightFace = (int(aLighting) >> 8) & 3;
	lampLightFace = (int(aLighting) >> 4) & 15;

	texcoord = aUV / textureAtlasSize;
	normal = tbNormal[( int(aNormal) >> 11) & 7];
}