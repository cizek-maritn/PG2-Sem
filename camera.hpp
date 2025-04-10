#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

enum CamMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:

    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Right; 
    glm::vec3 Up; // camera local UP vector
    glm::vec3 WorldUp;

    GLfloat Yaw;
    GLfloat Pitch;
    GLfloat Roll;
    GLfloat FOV=60.0f;
    
    // Camera options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom;

    GLfloat Sprint;

    Camera(glm::vec3 position);

    glm::mat4 getViewMatrix();
    glm::mat4 getProjMatrix();

    void processInput(GLFWwindow* window, GLfloat deltaTime);
    void processMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch);
    glm::vec3 getPosition();

private:
    void updateCameraVectors();
};
