#pragma once

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include <cerrno>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Texture.h>

//my libs
std::string get_file_contents(const char* filename);

class Shader
{
public:
	Shader();//emty
	Shader(const char* vertexFile, const char* fragmentFile);
	// Reference ID of the Shader Program
	GLuint ID = 0;
	// Constructor that build the Shader Program from 2 different shaders

	// Activates the Shader Program
	void Bind()const;
	// Deletes the Shader Program
	void Delete();
	void UnBind()const;
	void SetVar(const char* name, int value) const;
	void SetVec3(const char* uniformName, glm::vec3 value)const;
	void SetVec4(const char* uniformName, glm::vec4 value)const;
	void SetMat4(const char* name, glm::mat4 matrix)const;
	void SetFloat(const char* name, float value) const {
		Bind();
		glUniform1f(glGetUniformLocation(ID, name), value);
	};
private:
	// Checks if the different Shaders have compiled properly
	void compileErrors(unsigned int shader, const char* type);
};