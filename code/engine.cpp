#define STB_IMAGE_IMPLEMENTATION

/*glad and glfw for OpenGL*/
#include <glad/glad.h>
#include <glfw3.h>


/*glm matrix and vectors*/
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

/*other files/translation units*/
#include "shader.h"
#include "camera.h"
#include "model.h"

#include <stdio.h>
#include <stdlib.h>


typedef signed short int int16;
typedef unsigned int uint16;

static GLFWwindow* window;
static int width = 800;
static int height = 600;

static float last_x = width / 2.0f; 
static float last_y = height / 2.0f;
static bool first_mouse = true;

static float delta_time = 0.0f;
static float last_frame = 0.0f;

static bool isFlashOn = true;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void process_input(GLFWwindow* window);

shader light_shader;
shader cube_shader;
shader model_shader;
camera cameras;
Model models;
Buffer buffer;
Texture texture;


static glm::vec3 objectPos = glm::vec3(1.0f,  0.0f, 0.0f);
static glm::vec3 lightPos = glm::vec3(7.0f, 1.0f, -1.0f);
static glm::vec3 planePos = glm::vec3(1.0f, 0.0f, 2.0f);

enum texture_id
  {
    FIRST = 1,
    SECOND = 2,
    THIRD = 3
  };

int main()
{
  
  /*initializes glfw*/
  glfwInit();
  /*this requires the user to have opengl3.3. if not glfw fails to run.
   it also sets the profile to core.*/
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, "Engine", NULL, NULL);
  if (window == NULL)
    {
      printf("failed to create glfw window");
      glfwTerminate();
      return -1;
    }
  
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
     printf("failed to initialize GLAD");
     return -1;
   }

  glViewport(0, 0, width, height);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  CameraInitVec(&cameras);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  ShaderInit(&cube_shader, "e:/engine/shaders/lightvertex.vs", "e:/engine/shaders/lightfragment.fs");
  ShaderInit(&model_shader, "e:/engine/shaders/model_loading.vs", "e:/engine/shaders/model_loading.fs");


float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

 glm::vec3 pointLightPositions[] =
   {
     glm::vec3(0.7f, 0.2f, 2.0f),
     glm::vec3(2.3f, -3.3f, -4.0f),
     glm::vec3(-4.0f,  2.0f, -12.0f),
     glm::vec3( 0.0f,  0.0f, -3.0f)
   };

 
 unsigned int light_cube_VAO, VBO;
  
 glGenVertexArrays(1, &light_cube_VAO);
 glGenBuffers(1, &VBO);
 glBindVertexArray(light_cube_VAO);

 glBindBuffer(GL_ARRAY_BUFFER, VBO);
 glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
 glEnableVertexAttribArray(0);

 stbi_set_flip_vertically_on_load(true);
 
 glEnable(GL_DEPTH_TEST);
 
 modelInit(&models, &buffer, "e:/engine/models/7ligmas/ParkBenches/Models/obj/Bench_01_a.obj");
    
  while(!glfwWindowShouldClose(window))
    {

      float current_frame = glfwGetTime();
      delta_time = current_frame - last_frame;
      last_frame = current_frame;

      
      process_input(window);
      ProcessMouseMovement(&cameras, 0.0f, 0.0f, true);
      
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      Use(&model_shader);

      SetVec3(&model_shader, "viewPos", cameras.Position);
      SetFloat(&model_shader, "shininess", 32.0f);

      SetVec3(&model_shader, "point.position", pointLightPositions[0]);
      SetVec3(&model_shader, "point.ambient", 0.5f, 0.5f, 0.5f);
      SetVec3(&model_shader, "point.diffuse", 0.8f, 0.8f, 0.8f);
      SetVec3(&model_shader, "point.specular", 1.0f, 1.0f, 1.0f);
      SetFloat(&model_shader, "point.constant", 1.0f);
      SetFloat(&model_shader, "point.linear", 0.09f);
      SetFloat(&model_shader, "point.quadratic", 0.032f);
      

      glm::mat4 view = GetViewMatrix(&cameras);
      glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 100.0f);
      glm::mat4 model = glm::mat4(1.0f);
      SetMat4(&model_shader, "view", view);
      SetMat4(&model_shader, "projection", projection);

      model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
      model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
      SetMat4(&model_shader, "model", model);
      drawModel(&models, &buffer, &model_shader);

      
      Use(&cube_shader);
      SetMat4(&cube_shader, "view", view);
      SetMat4(&cube_shader, "projection", projection); 
      SetVec3(&cube_shader, "lightCubeColor", 0.0f, 0.5f, 0.0f);
 
      glBindVertexArray(light_cube_VAO); 
      
      model = glm::mat4(1.0f);
      model = glm::translate(model, pointLightPositions[0]);
      model = glm::scale(model, glm::vec3(0.2f));
      SetMat4(&cube_shader, "model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
			 
      glfwSwapBuffers(window);
      glfwPollEvents();
    }

  glDeleteVertexArrays(1, &light_cube_VAO);
  glDeleteBuffers(1, &VBO);

  glfwTerminate();
  return 0;

}

 void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);

};
void process_input(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
      glfwSetWindowShouldClose(window, true);
    }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
      ProcessKeyboard(&cameras, FORWARD, delta_time);
    }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
      ProcessKeyboard(&cameras, BACKWARD, delta_time);
    }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
      ProcessKeyboard(&cameras, LEFT, delta_time);
    }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
      ProcessKeyboard(&cameras, RIGHT, delta_time);
    }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
      ProcessKeyboard(&cameras, DOWN, delta_time);
    }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
      ProcessKeyboard(&cameras, UP, delta_time);
    }
  
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
  float x_pos = static_cast<float>(xposIn);
  float y_pos = static_cast<float>(yposIn);

    if (first_mouse)
    {
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }

    float x_offset = x_pos - last_x;
    float y_offset = last_y - y_pos; 

    last_x = x_pos;
    last_y = y_pos;

    ProcessMouseMovement(&cameras, x_offset, y_offset);
}
