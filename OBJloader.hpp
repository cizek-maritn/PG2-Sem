#pragma once
#ifndef OBJloader_H
#define OBJloader_H

#include <vector>
#include <glm/fwd.hpp>
#include "Loader.h"
#include "ShaderProgram.hpp"

bool loadOBJ(
	const char * path,
	std::vector < glm::vec3 > & out_vertices,
	std::vector < glm::vec2 > & out_uvs,
	std::vector < glm::vec3 > & out_normals
);

class OBJLoader : public Loader
{
public:
	OBJLoader(const std::filesystem::path& filename);
	Mesh getMesh(ShaderProgram shader, glm::vec3 const& origin, glm::vec3 const& orientation);
};

#endif
