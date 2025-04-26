#include <string>
#include<sstream>
#include <GL/glew.h> 
#include <glm/glm.hpp>

#include "OBJloader.hpp"
#include "LoggerClass.h"
#include "Mesh.hpp"
#include "assets.h"
#include "StringTools.h"

#define MAX_LINE_SIZE 255

OBJLoader::OBJLoader(const std::filesystem::path& filename)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;
	MeshContainer currentGroup;
	usesMTL = false;

	LoggerClass::info("Loading " + filename.string());

	FILE* file;
	fopen_s(&file, filename.string().c_str(), "r");

	// Kontrola existence souboru
	if (file == NULL) {
		LoggerClass::error("File " + filename.string() + " does not exists!");
		return;
	}

	int face_index = 0;

	while (1) {
		char lineHeader[MAX_LINE_SIZE];
		int res = fscanf_s(file, "%s", lineHeader, MAX_LINE_SIZE);

		if (res == EOF) {
			break;
		}

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.y, &uv.x);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "mtllib")==0) {
			usesMTL = true;
			char mtlFilename[MAX_LINE_SIZE];
			fscanf_s(file, "%s\n", mtlFilename, MAX_LINE_SIZE);

			std::filesystem::path mtlPath = filename.parent_path() / std::string(mtlFilename);
			loadMTL(mtlPath);
		}
		else if (strcmp(lineHeader, "usemtl")==0) {
			if (!currentGroup.vertices.empty()) {
				meshGroups.push_back(currentGroup);
			}

			char materialName[MAX_LINE_SIZE];
			fscanf_s(file, "%s\n", materialName, MAX_LINE_SIZE);

			currentGroup = MeshContainer();
			currentGroup.materialName = std::string(materialName);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			char buffer[MAX_LINE_SIZE];
			fgets(buffer, MAX_LINE_SIZE, file);

			// Parse the buffer as a string
			std::string line(buffer);

			ltrim(line);
			rtrim(line);

			face_index += 1;

			std::vector<std::string> tokens = split(line, ' ');
			int vertex_number = tokens.size();
			int vertexIndex[4] = {}, uvIndex[4] = {}, normalIndex[4] = {};

			for (int i = 0; i < tokens.size(); ++i) {
				auto values = split(tokens[i], '/');
				if (!values.empty()) {
					if (values.size() >= 1) vertexIndex[i] = std::stoi(values[0]);
					if (values.size() >= 2) uvIndex[i] = std::stoi(values[1]);
					if (values.size() >= 3) normalIndex[i] = std::stoi(values[2]);
				}
			}

			auto pushVertex = [&](int idx) {
				unsigned int vIdx = vertexIndex[idx];
				unsigned int uvIdx = uvIndex[idx];
				unsigned int nIdx = normalIndex[idx];

				currentGroup.vertices.push_back(temp_vertices[vIdx - 1]);
				currentGroup.uvs.push_back(temp_uvs.empty() ? glm::vec2(0.0f) : temp_uvs[uvIdx - 1]);
				currentGroup.normals.push_back(temp_normals.empty() ? glm::vec3(0.0f, 1.0f, 0.0f) : temp_normals[nIdx - 1]);
				currentGroup.indices.push_back(currentGroup.vertices.size() - 1);
				};

			if (vertex_number == 3) {
				pushVertex(0);
				pushVertex(1);
				pushVertex(2);
			}
			else if (vertex_number == 4) {
				pushVertex(0);
				pushVertex(1);
				pushVertex(2);
				pushVertex(0);
				pushVertex(2);
				pushVertex(3);
			}
			else {
				printf("Error: Face format not supported.\n");
				return;
			}
		}
	}

	std::ostringstream statistics, indicies;

	LoggerClass::info("Parsed model " + filename.string());

	statistics << "Vertices: " << temp_vertices.size() << ", ";
	statistics << "UVs: " << temp_uvs.size() << ", ";
	statistics << "Normals: " << temp_normals.size();

	indicies << "Vertex Indicies: " << vertexIndices.size() << ", ";
	indicies << "UV Indicies: " << uvIndices.size() << ", ";
	indicies << "Normal Indicies: " << normalIndices.size();

	LoggerClass::debug("Model Stats: " + statistics.str());
	LoggerClass::debug("Model Indicies:" + indicies.str());

	if (!currentGroup.vertices.empty()) {
		meshGroups.push_back(currentGroup);
	}

	fclose(file);
}

std::vector<Mesh> OBJLoader::getMesh(ShaderProgram shader, glm::vec3 const & origin, glm::vec3 const & orientation)
{
	for (auto& group : meshGroups) {
		std::vector<Vertex> vertexes;
		for (unsigned int i = 0; i < group.vertices.size(); ++i) {
			Vertex vertex(group.vertices[i], group.normals[i], group.uvs[i]);
			vertexes.push_back(vertex);
		}

		if (!usesMTL) { 
			Mesh mesh(GL_TRIANGLES, shader, vertexes, group.indices, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f),1.0f,false,0);
			meshes.push_back(mesh);
		}
		else {
			Material mat = materials[group.materialName];
			Mesh mesh(GL_TRIANGLES, shader, vertexes, group.indices, glm::vec3(0.0f), glm::vec3(0.0f), mat.ambient, mat.diffuse, mat.specular, mat.shininess,true,0);
			meshes.push_back(mesh);
		}

	}
	return meshes;
}

void OBJLoader::loadMTL(const std::filesystem::path& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		LoggerClass::error("Failed to open MTL file: " + filename.string());
		return;
	}

	std::string line;
	std::string currentName;
	Material currentMaterial;

	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string token;
		iss >> token;

		if (token == "newmtl") {
			if (!currentName.empty()) {
				materials[currentName] = currentMaterial;
			}
			iss >> currentName;
			currentMaterial = Material(); // Reset
		}
		else if (token == "Ka") {
			iss >> currentMaterial.ambient.r >> currentMaterial.ambient.g >> currentMaterial.ambient.b;
		}
		else if (token == "Kd") {
			iss >> currentMaterial.diffuse.r >> currentMaterial.diffuse.g >> currentMaterial.diffuse.b;
		}
		else if (token == "Ks") {
			iss >> currentMaterial.specular.r >> currentMaterial.specular.g >> currentMaterial.specular.b;
		}
		else if (token == "Ns") {
			iss >> currentMaterial.shininess;
		}
	}

	if (!currentName.empty()) {
		materials[currentName] = currentMaterial;
	}

	LoggerClass::info("Loaded materials from: " + filename.string());
}