#version 330 core

out vec4 FragColor;
in float ao;
in vec2 texcoord;
in vec3 normal;
in float lightFace;

uniform sampler2D tex;
uniform float aoStrength;
uniform float worldSunLevel;

const float maxLight = 0.94;
const float minLight = 0.06;

float waterTransparency = 0.85;

void main()
{
	vec4 texCol = texture(tex, texcoord);
	texCol.a *= waterTransparency;
	if(texCol.a < .5) {
		discard;
	}
	

	float lightLamp = 0.0;
	float lightSunFace = int(lightFace) & 15;
	float lightSun = min(lightSunFace, worldSunLevel);

	float faceLightLevel = max(lightSun, lightLamp) / 15;
	faceLightLevel = clamp(faceLightLevel,minLight,maxLight);
	
	// Ambient occlusion light value
	if(ao > 0.0) {
		float aoLevel = ao / 3.0;
		faceLightLevel = clamp(faceLightLevel * aoLevel * aoStrength,minLight,maxLight);
	}


	FragColor = vec4(texCol.rgb * faceLightLevel, texCol.a);
}