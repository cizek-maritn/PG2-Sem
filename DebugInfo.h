#pragma once

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>

class DebugInfo {
public:
	DebugInfo();
	bool available;
	static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
};