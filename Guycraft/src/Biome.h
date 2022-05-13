#pragma once
#include <string>

class Biome {
public:
	std::string name = "Unsigned Biome";
	//if noise M < moiseture is gen biome
	float moiseture = 0.0f;
	//if noise T < temperature is gen biome
	float temperature = 0.0f;
};