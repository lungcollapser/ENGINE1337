#define STB_IMAGE_IMPLEMENTATION

/*glad and glfw for OpenGL*/
#include <glad/glad.h>
#include <glfw3.h>
#include <stb_image.h>

/*glm matrix and vectors*/
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

/*other files/translation units*/
#include <shader.h>
#include <camera.h>

#include <time.h>
#include <cstdio>


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


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void process_input(GLFWwindow* window);

shader light_shader;
shader cube_shader;
camera cameras;


static glm::vec3 objectPos = glm::vec3(1.0f,  0.0f, 0.0f);
static glm::vec3 lightPos = glm::vec3(7.0f, 1.0f, -1.0f);
static glm::vec3 planePos = glm::vec3(1.0f, 0.0f, 2.0f);
static glm::vec3 diffuseColorChange;


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

  ShaderInit(&light_shader, "w:/shaders/vertex.vs", "w:/shaders/fragment.fs");
  ShaderInit(&cube_shader, "w:/shaders/lightvertex.vs", "w:/shaders/lightfragment.fs");


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
  
 uint16 VBO, lighting_VAO;
  glGenVertexArrays(1, &lighting_VAO);
  glGenBuffers(1, &VBO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(lighting_VAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  uint16 light_cube_VAO;
  
  glGenVertexArrays(1, &light_cube_VAO);
  glBindVertexArray(light_cube_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  
  uint16 containerTex, containerSpecTex, emissionTex;
  
  glGenTextures(1, &containerTex);

  glBindTexture(GL_TEXTURE_2D, containerTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  int width, height, nrChannels;
  unsigned char *containerData = stbi_load("w:/art/container2.png", &width, &height, &nrChannels, 0);
  if (containerData)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, containerData);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
  else
    {
      printf("failed to load texture\n");
    }
  stbi_image_free(containerData);
  
  glGenTextures(1, &containerSpecTex); 

  glBindTexture(GL_TEXTURE_2D, containerSpecTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


  unsigned char *containerSpecData = stbi_load("w:/art/container2_specular.png", &width, &height, &nrChannels, 0);
    if (containerSpecData)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, containerSpecData);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
  else
    {
      printf("failed to load texture\n");
    }
  stbi_image_free(containerSpecData);

  glGenTextures(1, &emissionTex);

  glBindTexture(GL_TEXTURE_2D, emissionTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  unsigned char *emissionData = stbi_load("w:/art/matrix.jpg", &width, &height, &nrChannels, 0);
  if (emissionData)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, emissionData);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
  else
    {
      printf("failed to load texture\n");
    }
  stbi_image_free(emissionData);
  
  Use(&light_shader);
  SetInt(&light_shader, "material.diffuse", 0); 
  SetInt(&light_shader, "material.specular", 1);
  SetInt(&light_shader, "material.emission", 2);
    
  while(!glfwWindowShouldClose(window))
    {
      
      float current_frame = glfwGetTime();
      delta_time = current_frame - last_frame;
      last_frame = current_frame;

      diffuseColorChange.x = sin(glfwGetTime()) * 7.9f + 2.0f;
      diffuseColorChange.y = sin(glfwGetTime()) * 3.3f;
      diffuseColorChange.z = sin(glfwGetTime()) * 2.2f;
      
      process_input(window);
      ProcessMouseMovement(&cameras, 0.0f, 0.0f, true);
      
      glEnable(GL_DEPTH_TEST);
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      Use(&light_shader);
      SetVec3(&light_shader, "light_pos", lightPos.x, lightPos.y, lightPos.z);
      SetVec3(&light_shader, "view_pos", cameras.Position.x, cameras.Position.y, cameras.Position.z);

      SetVec3(&light_shader, "light.ambient", 0.2f, 0.2f, 0.2f);
      SetVec3(&light_shader, "light.diffuse", 0.4f, 0.4f, 0.4f);
      SetVec3(&light_shader, "light.specular", 1.0f, 1.0f, 1.0f);
      SetVec3(&light_shader, "light.emission", 1.0f, 1.0f, 1.0f);

      SetVec3(&light_shader, "material.diffuseColor", diffuseColorChange.x, diffuseColorChange.y, diffuseColorChange.z);
      SetFloat(&light_shader, "material.shininess", 64.0f);
      SetVec2(&light_shader, "material.scale", 0.5f, 0.5f);
      
      glm::mat4 view = glm::mat4(1.0f);
      view = GetViewMatrix(&cameras);
      
      glm::mat4 projection = glm::mat4(1.0f);
      projection = glm::perspective(glm::radians(60.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
      
      glm::mat4 model = glm::mat4(1.0f);
      SetMat4(&light_shader, "view", view);
      SetMat4(&light_shader, "projection", projection); 

      model = glm::mat4(1.0f);
      model = glm::translate(model, objectPos);
      SetMat4(&light_shader, "model", model);


      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, containerTex);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, containerSpecTex);

      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, emissionTex);
      
      glBindVertexArray(lighting_VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      Use(&cube_shader);
      SetMat4(&cube_shader, "view", view);
      SetMat4(&cube_shader, "projection", projection);

      model = glm::mat4(1.0f);
      model = glm::translate(model, lightPos);
      model = glm::rotate(model, (float)glfwGetTime() * glm::radians(90.0f), glm::vec3(1.0f, 2.4f, 2.1f));
      model = glm::scale(model, glm::vec3(0.3f));        
     
      SetMat4(&cube_shader, "model", model);
      
      glBindVertexArray(light_cube_VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);
	
      
      glfwSwapBuffers(window);
      glfwPollEvents();
    }

  glDeleteVertexArrays(1, &lighting_VAO);
  glDeleteVertexArrays(2, &light_cube_VAO);
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
    float y_offset = last_y - y_pos; // reversed since y-coordinates go from bottom to top

    last_x = x_pos;
    last_y = y_pos;

    ProcessMouseMovement(&cameras, x_offset, y_offset);
}

