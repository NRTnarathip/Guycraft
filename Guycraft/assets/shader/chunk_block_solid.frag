#version 330 core

out vec4 FragColor;
in vec2 texcoord;
in vec3 normal;
in float sunLightFace;
in float aoLightFace;
in float lampLightFace;

uniform sampler2D tex;
uniform float aoStrength;
uniform float worldSunLevel;


const float maxLight = 0.94;
const float minLight = 0.06;

void main()
{
	vec4 texCol = texture(tex, texcoord);
	if(texCol.a < .5) {
		discard;
	}
	
	float lightSun = min(sunLightFace, worldSunLevel);

	float faceLightLevel = max(lightSun, lampLightFace) / 15;
	faceLightLevel = clamp(faceLightLevel,minLight,maxLight);
	float aoLevel = (aoLightFace / 3.0) * aoStrength;
	faceLightLevel = clamp(faceLightLevel - aoLevel, minLight,maxLight);


	FragColor = vec4(texCol.rgb * faceLightLevel, texCol.a);
}