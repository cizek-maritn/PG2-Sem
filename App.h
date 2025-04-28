#pragma once

#include <opencv2/opencv.hpp>
#include "assets.h"
#include "WindowClass.h"
#include "Model.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "PointLight.h"
#include <vector>
#include <map>
#include <unordered_map>

class App {
public:
	App();

	bool init(void);
	void init_assets(void);
	void report(void);
	int run(void);
	GLuint gen_tex(cv::Mat& image);
	GLuint textureInit(const std::filesystem::path& file_name);
	void init_hm(void);
	void init_pl(void);
	Mesh GenHeightMap(const cv::Mat& hmap, const unsigned int mesh_step_size);
	glm::vec2 get_subtex_st(const int x, const int y);
	glm::vec2 get_subtex_by_height(float height);
	float getHeightMapY(float x, float z);

	~App();

protected:
	std::unordered_map<std::string, Model> scene;
private:
	WindowClass* window;

	//new GL stuff
	GLuint shader_prog_ID{ 0 };
	GLuint VBO_ID{ 0 };
	GLuint VAO_ID{ 0 };
	GLuint hmapTex;

	ShaderProgram my_shader;
	ShaderProgram hmapShader;

	Mesh heightMap;
	//used for getting Y coordinate for height map collision
	cv::Mat heightMapData;

	std::vector<PointLight> pointLights;
};