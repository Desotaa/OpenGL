#include "Shader.h"

//Going to read shaders from the files
Shader::Shader(const char * vertexPath, const char * fragmentPath)
{
	//Retrieve and store the shader codes
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	//Ensure ifstream objects can throw exceptions. Set the exception masks
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		//Open the files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		//Read file's buffer contents into the streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//Close the files
		vShaderFile.close();
		fShaderFile.close();
		//Extract the strings from the streams
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure fail)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ->" << fail.what() << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//COMPILE THE SHADERS
	unsigned int vertex, fragment;
	//Vertex Shader Compilation
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	//Check errors
	checkCompileErrors(vertex, "VERTEX");

	//Compile the fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	//Check errors
	checkCompileErrors(fragment, "FRAGMENT");
	//Create the shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	//Check linking errors
	checkCompileErrors(ID, "PROGRAM");

	//After linking the program we dont need shaders anymore.
	glDeleteShader(vertex);
	glDeleteShader(fragment);

}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string & name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string & name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string & name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string & name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string & name, const glm::vec3 & value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string & name, const glm::vec4 & value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setMat3(const std::string & name, const glm::mat3 & matrix) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setMat4(const std::string & name, const glm::mat4 & matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

unsigned int Shader::getID() const
{
	return ID;
}

void Shader::checkCompileErrors(unsigned int IDtoCheck, std::string type) const
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM") 
	{
		//Check shader errors
		glGetShaderiv(IDtoCheck, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(IDtoCheck, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type " << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		//Check program errors
		glGetProgramiv(IDtoCheck, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(IDtoCheck, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type " << type << "\n" << infoLog << std::endl;
		}

	}
}

