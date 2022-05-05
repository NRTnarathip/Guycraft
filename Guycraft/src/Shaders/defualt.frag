#version 330 core

out vec4 FragColor;
in float ao;
in vec2 texcoord;
in vec3 normal;
in float sunLampLight;

uniform sampler2D tex;
uniform vec3 sunDirect;
uniform float sunIntensity;
uniform float aoStrength;
uniform float fogMin;
uniform float fogMax;

void main()
{
	vec4 texCol = texture(tex, texcoord);
	if(texCol.a < .5) {
		discard;
	}
	float maxLight = 0.98;
	float minLight = 0.02;

	float lightLamp = 15.0;
	float lightSun = 15.0;
	float maxLightLampSun = max(lightSun,lightLamp) / 15;

	float light = maxLight;
	light = min(light,maxLight); // limit max light
	// Ambient occlusion light value
	if(ao > 0.0) {
		float aoTemp = ao / 3.0;
		aoTemp *= aoStrength;
		light -= aoTemp;
	}
	FragColor = vec4(texCol.rgb * light, texCol.a);

	
	float fog = clamp(((gl_FragCoord.z / gl_FragCoord.w) - fogMin) / (fogMax - fogMin), 0.0, 1.0);
	vec3 fogColor = vec3(0.4,0.4,0.4);
	FragColor = mix(FragColor, vec4(fogColor, 1.0), fog); // apply fog
}