#ifndef DYNAMIC_H
#define DYNAMIC_H

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  float *items;
  size_t count;
  size_t capacity;
} Vertices;


#define da_append(vert, x)\
  do\
{\
  if (vert.count >= vert.capacity)\
    {\
       if (vert.capacity == 0) vert.capacity = 256;\
       else vert.capacity *= 2;\
       vert.items = realloc(vert.items, vert.capacity * sizeof(*vert.items));\
    }\
    vert.items[vert.count++] = x;\			
} while(0)


void vert_array_init()
{
  Vertices vert = {0};
  for (int x = 0; x < 10; x++)
    {
      da_append(vert, x);
    }
  for (size_t i = 0; i < vert.count; i++)
    {
      printf("%f\n", vert.items[i]);
    }
}

#endif 
th
