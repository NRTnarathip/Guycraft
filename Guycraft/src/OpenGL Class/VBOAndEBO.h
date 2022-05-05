#pragma once
#include <glad/glad.h>

class HelperBufferObject {
public:
	~HelperBufferObject() {
		deleteBuffer();
	}
	unsigned int m_ID;
	unsigned int getID() {
		return m_ID;
	}
	void deleteBuffer() {
		glDeleteBuffers(1, &m_ID);
	}
	void genBuffer() {
		glGenBuffers(1, &m_ID);//element Buffer Object
	}
};
class VBO {
public:
	~VBO() {
		m_VBO.deleteBuffer();
	}
	HelperBufferObject m_VBO;
	void setupGL() {
		m_VBO.genBuffer();
	}
	void unbind() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void bind() {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO.m_ID);
	}
	void bufferData(GLsizeiptr size, const void* data, GLenum usage) {
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}
};

class EBO {
public:
	~EBO() {
		m_EBO.deleteBuffer();
	}
	HelperBufferObject m_EBO;
	void setupGL() {
		m_EBO.genBuffer();
	}
	void bind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO.m_ID);
	}
	void unbind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void bufferData(GLsizeiptr size, const void* data, GLenum usage) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
	}
};