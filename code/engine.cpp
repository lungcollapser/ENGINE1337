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


static float mix_value;
static glm::vec3 objectPos = glm::vec3(1.0f,  0.0f, 0.0f);
static glm::vec3 lightPos = glm::vec3(5.0f, 100.0f, -3.0f);
static glm::vec3 planePos = glm::vec3(1.0f, 0.0f, 2.0f);


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
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

  
 uint16 VBO, lighting_VAO;
  glGenVertexArrays(1, &lighting_VAO);
  glGenBuffers(1, &VBO);
  
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(lighting_VAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  uint16 light_cube_VAO;
  
  glGenVertexArrays(1, &light_cube_VAO);
  glBindVertexArray(light_cube_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  
  glm::vec3 size_value = glm::vec3(1.0f);
  
    
  while(!glfwWindowShouldClose(window))
    {
      
      float current_frame = glfwGetTime();
      delta_time = current_frame - last_frame;
      last_frame = current_frame;
      
      process_input(window);
      ProcessMouseMovement(&cameras, 0.0f, 0.0f, true);
      
      glEnable(GL_DEPTH_TEST);
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      /*
      lightPos.x = 2.0f + sin(glfwGetTime()) * 2.0f;
      lightPos.y = sin(glfwGetTime()) * -2.0f;
      lightPos.z = 2.0f + cos(glfwGetTime()) * 4.0f;
      
      glm::vec3 light_color;
      light_color.x = sin(glfwGetTime() * 2.0f);
      light_color.y = sin(glfwGetTime() * 1.0);
      light_color.z = sin(glfwGetTime() * 1.3f);
      */
      
      glm::vec3 diffuse_color = light_color * glm::vec3(0.5f);
      glm::vec3 ambient_color = diffuse_color * glm::vec3(0.2f);

      Use(&light_shader);
      SetVec3(&light_shader, "light_pos", lightPos.x, lightPos.y, lightPos.z);
      SetVec3(&light_shader, "view_pos", cameras.Position.x, cameras.Position.y, cameras.Position.z);

      SetVec3(&light_shader, "light.ambient", 1.0f, 1.0f, 1.0f);
      SetVec3(&light_shader, "light.diffuse", 1.0f, 1.0f, 1.0f);
      SetVec3(&light_shader, "light.specular", 1.0f, 1.0f, 1.0f);
      
      glm::mat4 view = glm::mat4(1.0f);
      glm::mat4 projection = glm::mat4(1.0f);
      view = GetViewMatrix(&cameras);
      projection = glm::perspective(glm::radians(60.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
      SetMat4(&light_shader, "view", view);
      SetMat4(&light_shader, "projection", projection); 
      
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, objectPos);
      model = glm::rotate(model, (float)glfwGetTime() * glm::radians(120.0f), glm::vec3(1.0f, 4.2f, 2.7f));
      SetMat4(&light_shader, "model", model);
      SetVec3(&light_shader, "material.ambient", 0.0215f, 0.1745f, 0.0215f);
      SetVec3(&light_shader, "material.diffuse", 0.07568f, 0.61424f, 0.07568f);
      SetVec3(&light_shader, "material.specular", 0.633f, 0.727811f, 0.633f);
      SetFloat(&light_shader, "material.shininess", 0.6f);
      
      glBindVertexArray(lighting_VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      Use(&light_shader);
      model = glm::mat4(1.0f);
      model = glm::translate(model, planePos);
      model = glm::rotate(model, (float)glfwGetTime() * glm::radians(120.0f), glm::vec3(1.0f, 4.2f, 2.7f));
      SetMat4(&light_shader, "model", model);
      SetVec3(&light_shader, "material.ambient", 0.5f, 0.0f, 0.0f);
      SetVec3(&light_shader, "material.diffuse", 0.5f, 0.4f, 0.7f);
      SetVec3(&light_shader, "material.specular", 0.7f, 0.04f, 0.04f);
      SetFloat(&light_shader, "material.shininess", 0.078125f);
      
      glBindVertexArray(lighting_VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      Use(&light_shader);
      model = glm::mat4(1.0f);
      model = glm::translate(model, planePos * 2.5f);
      model = glm::rotate(model, (float)glfwGetTime() * glm::radians(120.0f), glm::vec3(1.0f, 4.2f, 2.7f));
      SetMat4(&light_shader, "model", model);
      SetVec3(&light_shader, "material.ambient", 0.5f, 0.0f, 0.0f);
      SetVec3(&light_shader, "material.diffuse", 0.5f, 0.4f, 0.7f);
      SetVec3(&light_shader, "material.specular", 0.7f, 0.04f, 0.04f);
      SetFloat(&light_shader, "material.shininess", 0.078125f);
      
      glBindVertexArray(lighting_VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);


      Use(&cube_shader);
      SetMat4(&cube_shader, "view", view);
      SetMat4(&cube_shader, "projection", projection);

      model = glm::mat4(1.0f);
      model = glm::translate(model, lightPos);

      model = glm::scale(model, glm::vec3(10.0f));       
      SetMat4(&light_shader, "light_rotation", glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 2.0f, 1.0f)));
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

