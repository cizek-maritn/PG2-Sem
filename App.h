#pragma once

#include "assets.h"
#include "WindowClass.h"
#include <vector>

class App {
public:
	App();

	bool init(void);
    void init_assets(void);
	void report(void);
	int run(void);

	~App();
private:
	WindowClass* window;

    //new GL stuff
    GLuint shader_prog_ID{ 0 };
    GLuint VBO_ID{ 0 };
    GLuint VAO_ID{ 0 };

    std::vector<vertex> triangle_vertices =
    {
        {{0.0f,  0.5f,  0.0f}},
        {{0.5f, -0.5f,  0.0f}},
        {{-0.5f, -0.5f,  0.0f}}
    };
};