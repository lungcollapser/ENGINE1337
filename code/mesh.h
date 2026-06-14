#ifndef MESH_H
#define MESH_H

#include <glfw3.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "shader.h"



struct Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

struct Texture
{
  unsigned int id;
  std::string type;
  std::string path;

  float indexX;
  float indexY;
  float sheetHeight;
  float sheetWidth;

  float spriteHeight;
  float spriteWidth;
  std::vector<glm::vec2> texturePositions;
};
struct Buffer
{
  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;
};
struct Mesh
{
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  Buffer buffer;
  
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
  {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    meshInit();
  }
  void meshInit()
  {
    
    glGenVertexArrays(1, &buffer.VAO);
    glGenBuffers(1, &buffer.VBO);
    glGenBuffers(1, &buffer.EBO);

    glBindVertexArray(buffer.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);


  }
};

void drawMesh(Mesh *mesh, shader* shaderID)
{

  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  for (unsigned int i = 0; i < mesh->textures.size(); i++)
    {
      glActiveTexture(GL_TEXTURE0 + i);

      std::string number;
      std::string name = mesh->textures[i].type;
      if (name == "texture_diffuse")
	{
	  number = std::to_string(diffuseNr++);
	}
      else if (name == "texture_specular")
	{
	  number = std::to_string(specularNr++);
	}

      SetInt(shaderID, ("material." + name + number), i);
      glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
  
    }

  glBindVertexArray(mesh->buffer.VAO);
  glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glActiveTexture(GL_TEXTURE0);
}


#endif
