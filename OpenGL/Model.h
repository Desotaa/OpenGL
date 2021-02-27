#pragma once
#ifndef MODEL_H
#define MODEL_H
#include <glad/glad.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>

GLuint textureFromFile(const char *filename, const std::string& directory, bool verticalFlip);

class Model
{
public:
	Model(const char *path);

	void draw(Shader &shader) const;
private:
	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *material, aiTextureType type, const std::string &typeName);

	//Model data
	std::vector<Mesh> meshes;
	//The directory where the model file lies
	std::string directory;
	std::unordered_map<std::string, bool> loaded_textures;
};
#endif