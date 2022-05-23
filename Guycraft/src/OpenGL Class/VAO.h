#pragma once
#include <glad/glad.h>
#include <algorithm>

class VAO {
public:
	unsigned int id;
	~VAO() {
		deleteArray();
	}
	void deleteArray() {
		glDeleteVertexArrays(1, &id);
	}
	void genBuffer() {
		glGenVertexArrays(1, &id);//Vertex Array Object
	};
	void bind() {
		glBindVertexArray(id);
	}
	void unbind() {
		glBindVertexArray(0);
	}
};