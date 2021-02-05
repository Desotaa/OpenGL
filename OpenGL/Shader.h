#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);
	// use/activate the shader
	void use();
	// utility uniform functions. Note that to call these functions, first you have to activate the shader program
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMat4(const std::string& name, const glm::mat4& matrix);
	unsigned int getID() const;
private:
	void checkCompileErrors(unsigned int shader, std::string type) const;
private:
	// the program ID
	unsigned int ID;

};

#endif