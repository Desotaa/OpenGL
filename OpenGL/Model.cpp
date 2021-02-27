#include "Model.h"

//You have to include STB in the cpp file. If you include it in the header file, other header files
//will also try to include stbi so it will create duplicate symbols
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Model::Model(const char * path)
{
	loadModel(path);
}

void Model::draw(Shader & shader) const
{
	for (int i = 0; i < meshes.size(); ++i)
	{
		meshes[i].draw(shader);
	}
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	//Parsing directory for later use
	directory = path.substr(0, path.find_last_of("/"));

	processNode(scene->mRootNode, scene);

}

void Model::processNode(aiNode * node, const aiScene * scene)
{
	//Process all the meshes in the current node
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	//Process childrens
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene);
	}

}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//Traverse vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		//Positions
		glm::vec3 temp;
		//mVertices stores positions in Assimp's Mesh formation. (Always present in meshes)
		temp.x = mesh->mVertices[i].x;
		temp.y = mesh->mVertices[i].y;
		temp.z = mesh->mVertices[i].z;
		vertex.position = temp;
		//Normals
		if (mesh->HasNormals())
		{
			temp.x = mesh->mNormals[i].x;
			temp.y = mesh->mNormals[i].y;
			temp.z = mesh->mNormals[i].z;
			vertex.normal = temp;
		}
		//Texture Coordinates
		if (mesh->mTextureCoords[0]) //If exists
		{
			glm::vec2 tex;
			//Assimp allows a model to have up to 8 different texture coordinates per vertex. We are only interested in the first set.
			tex.x = mesh->mTextureCoords[0][i].x;
			tex.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = tex;
		}
		else
		{
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	//Indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	//A mesh can only contain 1 material in assimp if there are more than 1 materials, Assimp
	//divides mesh into submeshes and stores in the same node.
	//Material. 
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial * material, aiTextureType type, const std::string & typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString str;
		material->GetTexture(type, i, &str);//Material stores the texture filenames
		if (loaded_textures.count(str.C_Str()) == 0) //Texture has not been loaded before
		{
			Texture texture;
			texture.id = textureFromFile(str.C_Str(), directory, true);
			texture.type = typeName;
			loaded_textures[str.C_Str()] = true;
			textures.push_back(texture);
		}
	}
	
	return textures;
}

GLuint textureFromFile(const char * filename, const std::string & directory, bool verticalFlip)
{
	std::string path = directory + "/" + std::string(filename);

	//Generate the texture
	GLuint textureId;
	glGenTextures(1, &textureId);

	//Load the data from the file
	stbi_set_flip_vertically_on_load(verticalFlip);
	int width, height, nrComponents;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		//Bind and send the data
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//Configure params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

	}
	else
	{
		std::cout << "Texture failed to load on path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureId;
}
