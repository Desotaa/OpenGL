#pragma once
#ifndef MESH_H
#define MESH_H
#include <glad/glad.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture
{
	GLuint id;
	std::string type;
};

class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices_in, const std::vector<GLuint>& indices_in, const std::vector<Texture>& textures_in);

	void draw(Shader& shader) const;

	//Getters
	std::vector<Vertex> getVertices() const;
	std::vector<GLuint> getIndices() const;
	std::vector<Texture> getTextures() const;

private:
	void setupMesh();
private:
	GLuint VAO, VBO, EBO;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
};

#endif