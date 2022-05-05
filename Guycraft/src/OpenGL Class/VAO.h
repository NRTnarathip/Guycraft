#pragma once
#include <glad/glad.h>

class VAO {
public:
	unsigned int m_VAO;
	~VAO() {
		deleteArray();
	}
	void deleteArray() {
		glDeleteVertexArrays(1, &m_VAO);
	}
	void setupGL() {
		glGenVertexArrays(1, &m_VAO);//Vertex Array Object
	};
	void bind() {
		glBindVertexArray(m_VAO);
	}
	void unbind() {
		glBindVertexArray(0);
	}
};