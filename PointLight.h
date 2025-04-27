#pragma once
#include <glm/glm.hpp> 
#include <glm/ext.hpp>


struct PointLight {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;
};