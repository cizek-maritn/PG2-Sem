#pragma once
#ifndef OBJloader_H
#define OBJloader_H

#include <vector>
#include <glm/fwd.hpp>
#include <unordered_map>
#include "Loader.h"
#include "ShaderProgram.hpp"

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

struct MeshContainer {
	std::string materialName;
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;
	std::vector<unsigned int> indices;
};

class OBJLoader : public Loader
{
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	std::unordered_map<std::string, Material> materials;
	std::string currentMaterialName;
	std::vector<MeshContainer> meshGroups;
	std::vector<Mesh> meshes;

	void loadMTL(const std::filesystem::path& filename);
public:
	OBJLoader(const std::filesystem::path& filename);
	std::vector<Mesh> getMesh(ShaderProgram shader, glm::vec3 const& origin, glm::vec3 const& orientation);
};

#endif
