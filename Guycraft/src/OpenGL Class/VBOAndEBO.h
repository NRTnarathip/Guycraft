#pragma once
#include <glad/glad.h>
#include <algorithm>

class HelperBufferObject {
public:
	~HelperBufferObject() {
		deleteBuffer();
	}
	unsigned int id;
	void deleteBuffer() {
		glDeleteBuffers(1, &id);
	}
	void genBuffer() {
		glGenBuffers(1, &id);//element Buffer Object
	}
};
class VBO : public HelperBufferObject{
public:
	~VBO() {
		deleteBuffer();
	}
	void unbind() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void bind() {
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}
	void bufferData(GLsizeiptr size, const void* data, GLenum usage) {
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}
};

class EBO : public HelperBufferObject {
public:
	~EBO() {
		deleteBuffer();
	}
	void bind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}
	void unbind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void bufferData(GLsizeiptr size, const void* data, GLenum usage) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
	}
};