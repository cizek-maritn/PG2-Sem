#ifndef OPENGL_WINDOW_H
#define OPENGL_WINDOW_H

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "camera.hpp"

class WindowClass {
public:
	WindowClass(int w, int h, const char* name, bool fullscreen = false, bool vsync = false);
	~WindowClass();

	void setFullscreen(bool fullscreen);
	void setVsync(bool vsync);

	void handleScrollEvent(double x, double y);
	void handleKeyEvent(int key, int action);
	void handleMouseButtonEvent(int button, int action);

	static void scrollCallback(GLFWwindow* window, double x, double y);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void mouseMovementCallback(GLFWwindow* window, double x, double y);

	glm::vec4 rgba = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	bool getFullscreen() const;
	bool getVsync() const;
	GLFWwindow* getWindow() const;

	static std::unique_ptr<Camera> cam;
	static bool mouseMove;
	static float lastX;
	static float lastY;

private:
	static void frameBufferSizeCallback(GLFWwindow* window, int w, int h);
	void handleKeyPress(int key, int action);
	void handleMousePress(int button, int action);
	GLFWwindow* window;

	bool fullscreen;
	bool vsync;

	int prevW;
	int prevH;
	int prevX;
	int prevY;

	void changeBlue(double val);
	void changeGreen(double val);
};

#endif