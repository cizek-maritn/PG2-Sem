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

struct MeshContainer {
	std::vector< unsigned int > vertices;
	std::vector< unsigned int > uvs;
	std::vector< unsigned int > normals;
};

//bool loadOBJ(const char * path, std::vector < glm::vec3 > & out_vertices, std::vector < glm::vec2 > & out_uvs, std::vector < glm::vec3 > & out_normals)
//{
//	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
//	std::vector< glm::vec3 > temp_vertices;
//	std::vector< glm::vec2 > temp_uvs;
//	std::vector< glm::vec3 > temp_normals;
//
//	out_vertices.clear();
//	out_uvs.clear();
//	out_normals.clear();
//
//	LoggerClass::info(std::string("Loading ") + path);
//
//	FILE * file;
//	fopen_s(&file, path, "r");
//	if (file == NULL) {
//		LoggerClass::error(std::string("File does not exist: ") + path);
//		return false;
//	}
//
//	while (1) {
//
//		char lineHeader[MAX_LINE_SIZE];
//		int res = fscanf_s(file, "%s", lineHeader, MAX_LINE_SIZE);
//		if (res == EOF) {
//			break;
//		}
//
//		if (strcmp(lineHeader, "v") == 0) {
//			glm::vec3 vertex;
//			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
//			temp_vertices.push_back(vertex);
//		}
//		else if (strcmp(lineHeader, "vt") == 0) {
//			glm::vec2 uv;
//			fscanf_s(file, "%f %f\n", &uv.y, &uv.x);
//			temp_uvs.push_back(uv);
//		}
//		else if (strcmp(lineHeader, "vn") == 0) {
//			glm::vec3 normal;
//			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
//			temp_normals.push_back(normal);
//		}
//		else if (strcmp(lineHeader, "f") == 0) {
//			std::string vertex1, vertex2, vertex3;
//			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
//			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
//			if (matches != 9) {
//				printf("File can't be read by simple parser :( Try exporting with other options\n");
//				return false;
//			}
//			vertexIndices.push_back(vertexIndex[0]);
//			vertexIndices.push_back(vertexIndex[1]);
//			vertexIndices.push_back(vertexIndex[2]);
//			uvIndices.push_back(uvIndex[0]);
//			uvIndices.push_back(uvIndex[1]);
//			uvIndices.push_back(uvIndex[2]);
//			normalIndices.push_back(normalIndex[0]);
//			normalIndices.push_back(normalIndex[1]);
//			normalIndices.push_back(normalIndex[2]);
//		}
//	}
//
//	// unroll from indirect to direct vertex specification
//	// sometimes not necessary, definitely not optimal
//
//	for (unsigned int u = 0; u < vertexIndices.size(); u++) {
//		unsigned int vertexIndex = vertexIndices[u];
//		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
//		out_vertices.push_back(vertex);
//	}
//	for (unsigned int u = 0; u < uvIndices.size(); u++) {
//		unsigned int uvIndex = uvIndices[u];
//		glm::vec2 uv = temp_uvs[uvIndex - 1];
//		out_uvs.push_back(uv);
//	}
//	for (unsigned int u = 0; u < normalIndices.size(); u++) {
//		unsigned int normalIndex = normalIndices[u];
//		glm::vec3 normal = temp_normals[normalIndex - 1];
//		out_normals.push_back(normal);
//	}
//
//	fclose(file);
//	return true;
//}

OBJLoader::OBJLoader(const std::filesystem::path& filename)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

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
		else if (strcmp(lineHeader, "f") == 0) {
			char buffer[MAX_LINE_SIZE];
			fgets(buffer, MAX_LINE_SIZE, file);

			// Parse the buffer as a string
			std::string line(buffer);

			ltrim(line);
			rtrim(line);

			face_index += 1;

			int vertexIndex[4], uvIndex[4], normalIndex[4];
			std::vector<std::string> _tokens = split(line, ' ');
			std::vector<std::string> tokens;
			for (int i = 0; i < _tokens.size(); ++i) {
				if (std::all_of(_tokens[i].begin(), _tokens[i].end(), [](unsigned char c) { return std::isspace(c); })) continue;

				tokens.push_back(_tokens[i]);
			}

			// How many vertexes does the face have?
			int vertex_number = tokens.size();

			// For each vertex/uv/normal in face, parse it
			for (int i = 0; i < tokens.size(); ++i) {
				auto token = tokens[i];
				auto values = split(token, '/');

				for (int j = 0; j < values.size(); ++j) {
					int number;
					std::istringstream iss(values[j]);

					// String to integer conversion was successfull
					if (iss >> number) {
						//Hele tohle pùjde urèitì udìlat líp
						if (j == 0) vertexIndex[i] = number;
						if (j == 1) uvIndex[i] = number;
						if (j == 2) normalIndex[i] = number;
					}
				}
			}

			if (vertex_number == 3) {
				// Handle triangular face
				for (int i = 0; i < 3; ++i) {
					vertexIndices.push_back(vertexIndex[i]);
					uvIndices.push_back(uvIndex[i]);
					normalIndices.push_back(normalIndex[i]);
				}
			}
			else if (vertex_number == 4) {
				// Handle quad face
				// Convert quad into two triangles
				for (int i = 0; i < 3; ++i) {
					vertexIndices.push_back(vertexIndex[i]);
					uvIndices.push_back(uvIndex[i]);
					normalIndices.push_back(normalIndex[i]);
				}
				for (int i = 2; i < 5; ++i) {
					vertexIndices.push_back(vertexIndex[i % 4]);
					uvIndices.push_back(uvIndex[i % 4]);
					normalIndices.push_back(normalIndex[i % 4]);
				}
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

	// unroll from indirect to direct vertex specification
	// sometimes not necessary, definitely not optimal
	for (unsigned int u = 0; u < vertexIndices.size(); u++) {
		unsigned int vertexIndex = vertexIndices[u];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		vertices.push_back(vertex);
	}

	if (!temp_uvs.empty()) {
		for (unsigned int u = 0; u < uvIndices.size(); u++) {
			unsigned int uvIndex = uvIndices[u];
			glm::vec2 uv = temp_uvs[uvIndex - 1];
			uvs.push_back(uv);
		}
	}

	if (!temp_normals.empty()) {
		for (unsigned int u = 0; u < normalIndices.size(); u++) {
			unsigned int normalIndex = normalIndices[u];
			glm::vec3 normal = temp_normals[normalIndex - 1];
			normals.push_back(normal);
		}
	}

	// Iterate through the vertices and assign indices sequentially
	for (unsigned int i = 0; i < vertices.size(); ++i) {
		indices.push_back(i);
	}

	fclose(file);
}

Mesh OBJLoader::getMesh(ShaderProgram shader, glm::vec3 const & origin, glm::vec3 const & orientation)
{
	//Hey, i know it's spelled wrong.
	//But I've vertices already defined.
	std::vector<Vertex> vertexes;

	for (unsigned int i = 0; i < vertices.size(); i++) {
		Vertex vertex;
		vertex.Position = vertices[i];
		vertex.TexCoords = uvs[i];
		vertex.Normal = normals[i];

		vertexes.push_back(vertex);
	}

	std::cout << "V: " << vertices.size() << "\tI: " << indices.size() << std::endl;
	return Mesh(GL_TRIANGLES, shader, vertexes, indices, origin, orientation, 0);
}