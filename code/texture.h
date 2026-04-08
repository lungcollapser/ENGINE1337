#define STB_IMAGE_IMPLEMENTATION

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image.h>
#include <cstdio>


void load_texture(unsigned int texture, char texLocation[])
{

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nrChannels;
  unsigned char *data = stbi_load(texLocation, &width, &height, &nrChannels, 0);
  if (data)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
      printf("SUCCESS");
    }
  else
    {
      printf("failed to load texture\n");
    }
  stbi_image_free(data);
};


#endif
