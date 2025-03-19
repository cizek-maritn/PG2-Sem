#pragma once

#include "assets.h"
#include "WindowClass.h"
#include "Model.hpp"
#include "ShaderProgram.hpp"
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

	~App();

protected:
	std::unordered_map<std::string, Model> scene;
private:
	WindowClass* window;

	//new GL stuff
	GLuint shader_prog_ID{ 0 };
	GLuint VBO_ID{ 0 };
	GLuint VAO_ID{ 0 };

	ShaderProgram my_shader;
};