#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include "camera.hpp"
#include <glm/common.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

Camera::Camera(glm::vec3 position):Position(position) {
	//this->Position = glm::vec3(0.0f, 0.0f, 3.0f);
	this->Front = glm::vec3(0.0f, 0.0f, -1.0f);
	this->Up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->Right = glm::vec3(0.0f, 0.0f, 0.0f);
	this->WorldUp = glm::vec3(0.0f, 0.0f, 0.0f);

	this->Yaw = -90.0f;
	this->Pitch = 30.0f;
	this->Roll = 30.0f;
	this->Zoom = 0.0f;
	this->FOV = 60.0f;

	this->MovementSpeed = 10.0f;
	this->Sprint = 3.0f;
	this->MouseSensitivity = 0.1f;

	this->updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::getProjMatrix() {
	//std::cout << FOV << " " << Position << " " << Front << std::endl;
	return glm::perspective(glm::radians(FOV), 1920.0f / 1080.0f, 0.1f, 100.0f);
}

void Camera::processInput(GLFWwindow* window, GLfloat deltaTime) {
	glm::vec3 direction{ 0 };

	float speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? Sprint : 1) * MovementSpeed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		this->Position += speed * this->Front;
		//std::cout << glm::to_string(this->Position) << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		this->Position -= speed * this->Front;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		this->Position -= glm::normalize(glm::cross(this->Front, this->Up)) * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		this->Position += glm::normalize(glm::cross(this->Front, this->Up)) * speed;
	}
}

void Camera::processMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch) {
	xoffset *= this->MouseSensitivity;
	yoffset *= this->MouseSensitivity;

	this->Yaw += xoffset;
	this->Pitch += yoffset;

	if (constraintPitch)
	{
		if (this->Pitch > 89.0f)
			this->Pitch = 89.0f;
		if (this->Pitch < -89.0f)
			this->Pitch = -89.0f;
	}

	this->updateCameraVectors();
}

void Camera::updateCameraVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	front.y = sin(glm::radians(this->Pitch));
	front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

	this->Front = glm::normalize(front);
	this->Right = glm::normalize(glm::cross(this->Front, glm::vec3(0.0f, 1.0f, 0.0f)));
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}