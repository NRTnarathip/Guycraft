#pragma once
struct Voxel {
public:
	unsigned char id;
	unsigned char type;
	Voxel() {
		id = 0;
		type = 0;
	}
};