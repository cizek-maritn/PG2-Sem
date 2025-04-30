#pragma once
#include <glm/glm.hpp> 
#include <glm/ext.hpp>

enum class OrbitMode {
	None,
	Clockwise,
	CounterClockwise
};

struct PointLight {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;

	OrbitMode orbitMode = OrbitMode::None;

	void orbit(float t) {
		if (orbitMode == OrbitMode::Clockwise) {
			position.x -= sin(t) * 0.01f;
			position.z -= cos(t) * 0.01f;
		}
		else if (orbitMode == OrbitMode::CounterClockwise) {
			position.x += sin(t) * 0.01f;
			position.z += cos(t) * 0.01f;
		}
	}
};