#ifndef OPENGL_WINDOW_H
#define OPENGL_WINDOW_H

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
#include <iostream>

class WindowClass {
public:
	WindowClass(int w, int h, const char* name, bool fullscreen = false, bool vsync = false);
	~WindowClass();

	void setFullscreen(bool fullscreen);
	void setVsync(bool vsync);

	void handleScrollEvent(double x, double y);
	void handleKeyEvent(int key, int action);
	void handleMouseEvent(int button, int action);

	static void scrollCallback(GLFWwindow* window, double x, double y);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseCallback(GLFWwindow* window, int button, int action, int mods);

	bool getFullscreen() const;
	bool getVsync() const;
	GLFWwindow* getWindow() const;

	GLfloat r{ 1.0f }, g{ 0.0f }, b{ 0.0f }, a{ 1.0f };

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