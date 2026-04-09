#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  4.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


struct camera
{
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
};


inline void updateCameraVectors(camera* cameraVal)
{

        glm::vec3 front;
        front.x = cos(glm::radians(cameraVal->Yaw)) * cos(glm::radians(cameraVal->Pitch));
        front.y = sin(glm::radians(cameraVal->Pitch));
        front.z = sin(glm::radians(cameraVal->Yaw)) * cos(glm::radians(cameraVal->Pitch));
        cameraVal->Front = glm::normalize(front);
        cameraVal->Right = glm::normalize(glm::cross(cameraVal->Front, cameraVal->WorldUp)); 
        cameraVal->Up = glm::normalize(glm::cross(cameraVal->Right, cameraVal->Front));
}
inline void CameraInitVec(camera* cameraVal) 
  {
      cameraVal->Position = glm::vec3(0.0f, 0.0f, 0.0f);
      cameraVal->WorldUp = glm::vec3(0.0f, 2.0f, 0.0);
      cameraVal->Yaw = YAW;
      cameraVal->Pitch = PITCH;
      cameraVal->Front = glm::vec3(0.0f, 0.0f, -1.0f);
      cameraVal->MovementSpeed = SPEED;
      cameraVal->MouseSensitivity = SENSITIVITY;
      updateCameraVectors(cameraVal);
  } 
inline void CameraInitScal(camera* cameraVal, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
{
      cameraVal->Position = glm::vec3(posX, posY, posZ);
      cameraVal->WorldUp = glm::vec3(upX, upY, upZ);
      cameraVal->Pitch = pitch;
      cameraVal->Front = glm::vec3(0.0f, 0.0f, -1.0f);
      cameraVal->MovementSpeed = SPEED;
      cameraVal->MouseSensitivity = SENSITIVITY;
      updateCameraVectors(cameraVal);
}

    glm::mat4 GetViewMatrix(camera* cameraVal)
    {
      return glm::lookAt(cameraVal->Position, cameraVal->Position + cameraVal->Front, cameraVal->Up);
    }


inline void ProcessKeyboard(camera* cameraVal, Camera_Movement direction, float deltaTime)
    {
        float velocity = cameraVal->MovementSpeed * deltaTime;
        if (direction == FORWARD)
	  {
            cameraVal->Position += cameraVal->Front * velocity;
	  }
        if (direction == BACKWARD)
	  {
            cameraVal->Position -= cameraVal->Front * velocity;
	  }
        if (direction == LEFT)
	  {
            cameraVal->Position -= cameraVal->Right * velocity;
	  }
        if (direction == RIGHT)
	  {
	     cameraVal->Position += cameraVal->Right * velocity;
	  }
	if (direction == UP)
	  {
	    cameraVal->Position += cameraVal->WorldUp * velocity;
	  }
	if (direction == DOWN)
	  {
	    cameraVal->Position -= cameraVal->WorldUp * velocity;
	  }
 
    }

   
inline void ProcessMouseMovement(camera* cameraVal, float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= cameraVal->MouseSensitivity;
        yoffset *= cameraVal->MouseSensitivity;

        cameraVal->Yaw   += xoffset;
        cameraVal->Pitch += yoffset;

        if (constrainPitch)
        {
            if (cameraVal->Pitch > 89.0f)
                cameraVal->Pitch = 89.0f;
            if (cameraVal->Pitch < -89.0f)
                cameraVal->Pitch = -89.0f;
        }

        updateCameraVectors(cameraVal);
    }

inline void ProcessMouseScroll(camera* cameraVal, float yoffset)
{
        cameraVal->Zoom -= (float)yoffset;
        if (cameraVal->Zoom < 1.0f)
            cameraVal->Zoom = 1.0f;
        if (cameraVal->Zoom > 45.0f)
            cameraVal->Zoom = 45.0f;
}


#endif
