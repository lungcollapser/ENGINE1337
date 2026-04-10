#define STB_IMAGE_IMPLEMENTATION

/*glad and glfw for OpenGL*/
#include <glad/glad.h>
#include <glfw3.h>


/*glm matrix and vectors*/
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

/*other files/translation units*/
#include <shader.h>
#include <camera.h>
#include <texture.h>

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

static bool isFlashOn = true;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void process_input(GLFWwindow* window);

shader light_shader;
shader cube_shader;
camera cameras;


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


 glm::vec3 cubePositions[] =
    {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
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


  load_texture(FIRST, "w:/art/container2.png");
  load_texture(SECOND, "w:/art/container2_specular.png");

  
  Use(&light_shader);
  SetInt(&light_shader, "material.diffuse", 0); 
  SetInt(&light_shader, "material.specular", 1);
    
  while(!glfwWindowShouldClose(window))
    {

      if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
	  isFlashOn = !isFlashOn;
	}

      
      float current_frame = glfwGetTime();
      delta_time = current_frame - last_frame;
      last_frame = current_frame;

      
      process_input(window);
      ProcessMouseMovement(&cameras, 0.0f, 0.0f, true);
      
      glEnable(GL_DEPTH_TEST);
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      Use(&light_shader);
      SetVec3(&light_shader, "view_pos", cameras.Position.x, cameras.Position.y, cameras.Position.z);

      SetVec3(&light_shader, "light.direction", -0.2f, -1.0f, -0.3f);
      SetVec3(&light_shader, "light.ambient", 0.2f, 0.2f, 0.2f);
      SetVec3(&light_shader, "light.diffuse", 0.4f, 0.4f, 0.4f);
      SetVec3(&light_shader, "light.specular", 1.0f, 1.0f, 1.0f);

      SetFloat(&light_shader, "light.constant", 1.0f);
      SetFloat(&light_shader, "light.linear", 0.09f);
      SetFloat(&light_shader, "light.quadratic", 0.032f);

      SetVec3(&light_shader, "light.position", cameras.Position.x, cameras.Position.y, cameras.Position.z);
      SetVec3(&light_shader, "light.direction", cameras.Front.x, cameras.Front.y, cameras.Front.z);
      SetFloat(&light_shader, "light.innerCone", glm::cos(glm::radians(12.5f)));
      SetFloat(&light_shader, "light.outerCone", glm::cos(glm::radians(17.5f)));
      
      SetInt(&light_shader, "flash", isFlashOn);

      SetFloat(&light_shader, "material.shininess", 32.0f);
      
      glm::mat4 view = GetViewMatrix(&cameras);
      glm::mat4 projection = glm::perspective(glm::radians(60.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
      glm::mat4 model = glm::mat4(1.0f);
      SetMat4(&light_shader, "view", view);
      SetMat4(&light_shader, "projection", projection);
      SetMat4(&light_shader, "model", model);
      
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, FIRST);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, SECOND);
      

      glBindVertexArray(lighting_VAO);

      for (unsigned int i = 0; i < 10; i++)
	{
	  model = glm::mat4(1.0f);
	  float angle = 10.0f * i;
	  model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	  model = glm::translate(model, cubePositions[i]);

	  SetMat4(&light_shader, "model", model);
	  
	  glDrawArrays(GL_TRIANGLES, 0, 36);

	}      

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
