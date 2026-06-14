#ifndef MODEL_H
#define MODEL_H

#include <glfw3.h>
#include <glad/glad.h>
#include <stb_image.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

#include <vector>
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>

#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

#include "mesh.h"

   

unsigned int TextureFromFile(Texture *texture, const char *path, const std::string &directory, bool gamma);

struct Model
{
  std::vector<Mesh> meshes;
  std::string directory;
  std::vector<Texture> textures_loaded;
};

std::vector<Texture> loadMaterialTextures(Model* model, aiMaterial *mat, aiTextureType type, std::string typeName)
{
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;
    for (unsigned int j = 0; j < model->textures_loaded.size(); j++)
      {
	if (std::strcmp(model->textures_loaded[j].path.data(), str.C_Str()) == 0)
	  {
	    textures.push_back(model->textures_loaded[j]);
	    skip = true;
	    break;	       	       
	  }
      }
    if(!skip)
      {
	Texture texture;
	texture.id = TextureFromFile(&texture, str.C_Str(), model->directory, false);
	texture.type = typeName;
	texture.path = str.C_Str();
	textures.push_back(texture);
	model->textures_loaded.push_back(texture);
      }
  }
return textures;
}

Mesh processMesh(Model* model, aiMesh* amesh, const aiScene* scene)
{

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  
  for (unsigned int i = 0; i < amesh->mNumVertices; i++)
  {
    Vertex vert;

    glm::vec3 vector;
    vector.x = amesh->mVertices[i].x;
    vector.y = amesh->mVertices[i].y;
    vector.z = amesh->mVertices[i].z;
    vert.position = vector;

    vector.x = amesh->mNormals[i].x;
    vector.y = amesh->mNormals[i].y;
    vector.z = amesh->mNormals[i].z;
    vert.normal = vector;

    if (amesh->mTextureCoords[0])
      {
	glm::vec2 vector2;
	vector2.x = amesh->mTextureCoords[0][i].x;
	vector2.y = amesh->mTextureCoords[0][i].y;
	vert.texCoords = vector2;
      }
    else
      {
	vert.texCoords = glm::vec2(0.0f, 0.0f);
      }
    vertices.push_back(vert);
  }

    for(unsigned int i = 0; i < amesh->mNumFaces; i++)
      {
	aiFace face = amesh->mFaces[i];
	for(unsigned int j = 0; j < face.mNumIndices; j++)
	  {
	    indices.push_back(face.mIndices[j]);
	  }
      }

    if (amesh->mMaterialIndex >= 0)
        {
	aiMaterial *material = scene->mMaterials[amesh->mMaterialIndex];

	
	std::vector<Texture> diffuseMaps = loadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	std::vector<Texture> specularMaps = loadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
      }
  
    return Mesh(vertices, indices, textures);
};

void processNode(Model *model, aiNode* node, const aiScene* scene)
{
  for (unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    aiMesh* amesh = scene->mMeshes[node->mMeshes[i]];
    model->meshes.push_back(processMesh(model, amesh, scene));
  }
  for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
      processNode(model, node->mChildren[i], scene); 
    }
};
void loadModel(Model* model, std::string const &path)
{
  Assimp::Importer import;
  const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << '\n';
    return;
  };    
  model->directory = path.substr(0, path.find_last_of('/'));
  processNode(model, scene->mRootNode, scene);
     
};
void modelInit(Model* model, Buffer* buffer, std::string const &path)
{
  loadModel(model, path);
};
void drawModel(Model* model, Buffer* buffer, shader* shader)
{

  for (unsigned int i = 0; i < model->meshes.size(); i++)
    {
      drawMesh(&model->meshes[i], shader);
    }
};

unsigned int TextureFromFile(Texture *texture, const char *path, const std::string &directory, bool gamma)
{

  std::vector<Texture> textures;
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
	  {
            format = GL_RED;
	  }
        else if (nrComponents == 3)
	  {
            format = GL_RGB;
	  }
        else if (nrComponents == 4)
	  {
            format = GL_RGBA;
	  }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


#endif
