#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices_in, const std::vector<GLuint>& indices_in, const std::vector<Texture>& textures_in)
	:
	vertices(vertices_in),
	indices(indices_in),
	textures(textures_in)
{	
	setupMesh();
}

void Mesh::draw(Shader & shader) const
{
	//Activate the shader
	shader.use();

	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	//Assign sampler uniforms
	for (unsigned int i = 0; i < textures.size(); ++i)
	{
		//Activate the corresponding texture unit
		glActiveTexture(GL_TEXTURE0 + i);

		//Set the uniform
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);

		shader.setInt("material." + name + number, i);
		//Bind the corresponding texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//Reset the activated texture unit for another call
	glActiveTexture(GL_TEXTURE0);
}

std::vector<Vertex> Mesh::getVertices() const
{
	return vertices;
}

std::vector<GLuint> Mesh::getIndices() const
{
	return indices;
}

std::vector<Texture> Mesh::getTextures() const
{
	return textures;
}

void Mesh::setupMesh()
{
	//Generate VAO and Buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//Bind VAO and VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Send the vertex data to the VBO
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	//Bind EBO and send the data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	//Configure Attributes
	//Position Attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	//Normal Attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	//Texture Attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	
	//Unbind VAO and Buffers
	glBindVertexArray(0);
	glBindBuffer(GL_VERTEX_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
