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

	// Orbit-related
	glm::vec3 orbitCenter;     // Where to orbit around
	float orbitRadius = 5.0f;
	float orbitSpeed = 1.0f;   // Radians per second
	float orbitAngle = 0.0f;
	OrbitMode orbitMode = OrbitMode::None;

	void orbit(float deltaTime) {
		if (orbitMode != OrbitMode::None) {
			float dir = (orbitMode == OrbitMode::Clockwise) ? -1.0f : 1.0f;

			orbitAngle += dir * orbitSpeed * deltaTime;

			position.x = orbitCenter.x + orbitRadius * cos(orbitAngle);
			position.z = orbitCenter.z + orbitRadius * sin(orbitAngle);
		}
	}
};