#pragma once

#include <string>
#include <filesystem>
#include <GL/glew.h> 
#include <glm/glm.hpp>
#include <vector>

#include "Mesh.hpp"

class Loader
{
public:
	const std::filesystem::path& path = "";

	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;
	std::vector< GLuint > indices;

};