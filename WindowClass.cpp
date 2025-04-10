#pragma once
#include "WindowClass.h"
#include "LoggerClass.h"
#include "camera.hpp"
//#include "App.h"

float WindowClass::lastX = 0.0f;
float WindowClass::lastY = 0.0f;
bool WindowClass::mouseMove = true;
std::unique_ptr<Camera> WindowClass::cam = nullptr;

WindowClass::WindowClass(int w, int h, const char* name, bool fullscreen, bool vsync, bool aa, int aa_level) : fullscreen(fullscreen), vsync(vsync) {
	if (!glfwInit()) {
		std::cerr << "GLFW not initialized\n";
		std::exit(-1);
	}

	if (aa) {
		glfwWindowHint(GLFW_SAMPLES, aa_level);  // Enable MSAA with the specified level
	}
	else {
		glfwWindowHint(GLFW_SAMPLES, 0);  // Disable MSAA
	}

	if (fullscreen) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		window = glfwCreateWindow(mode->width, mode->height, name, monitor, nullptr);
	}
	else {
		window = glfwCreateWindow(w, h, name, nullptr, nullptr);
	}

	if (!window) {
		std::cerr << "Failed to make GLFW window\n";
		glfwTerminate();
		std::exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	glfwSetCursorPosCallback(window, mouseMovementCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (aa) {
		glEnable(GL_MULTISAMPLE);
	}
}

WindowClass::~WindowClass() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

GLFWwindow* WindowClass::getWindow() const {
	return window;
}

void WindowClass::setFullscreen(bool fullscreen) {
	this->fullscreen = fullscreen;
	if (fullscreen) {
		glfwGetWindowSize(window, &prevW, &prevH);
		glfwGetWindowPos(window, &prevX, &prevY);
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
	}
	else {
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowMonitor(window, nullptr, prevX, prevY, prevW, prevH, GLFW_DONT_CARE);
	}
}

void WindowClass::setVsync(bool vsync) {
	this->vsync = vsync;
	if (vsync) {
		glfwSwapInterval(1);
		return;
	}
	else {
		glfwSwapInterval(0);
		return;
	}
}

bool WindowClass::getFullscreen() const {
	return fullscreen;
}

bool WindowClass::getVsync() const {
	return vsync;
}

void WindowClass::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	WindowClass* instance = static_cast<WindowClass*>(glfwGetWindowUserPointer(window));
	if (instance) {
		instance->handleKeyEvent(key, action);
	}
}

void WindowClass::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	WindowClass* instance = static_cast<WindowClass*>(glfwGetWindowUserPointer(window));
	if (instance) {
		instance->handleMouseButtonEvent(button, action);
	}
}

void WindowClass::scrollCallback(GLFWwindow* window, double x, double y) {
	WindowClass* instance = static_cast<WindowClass*>(glfwGetWindowUserPointer(window));
	if (instance) {
		instance->handleScrollEvent(x, y);
	}
}

void WindowClass::mouseMovementCallback(GLFWwindow* window, double x, double y) {
	if (WindowClass::cam == nullptr) {
		LoggerClass::warning("No camera assigned to window.");
		return;
	}

	float xpos = static_cast<float>(x);
	float ypos = static_cast<float>(y);

	if (WindowClass::mouseMove) {
		WindowClass::lastX = xpos;
		WindowClass::lastY = ypos;
		WindowClass::mouseMove = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	WindowClass::cam->processMouseMovement(xoffset, yoffset, GL_TRUE);
}

void WindowClass::frameBufferSizeCallback(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, w, h);
}

void WindowClass::handleKeyEvent(int key, int action) {
	switch (action) {
	case GLFW_PRESS:
		this->handleKeyPress(key, action);
		break;
	default:
		break;
	}
}

void WindowClass::handleKeyPress(int key, int action) {
	switch (key) {
	case GLFW_KEY_ESCAPE: {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	}
	case GLFW_KEY_V: {
		vsync = !vsync;
		setVsync(vsync);
		std::cout << "VSync " << (vsync ? "enabled" : "disabled") << std::endl;
		break;
	}
	case GLFW_KEY_F: {
		fullscreen = !fullscreen;
		setFullscreen(fullscreen);
		std::cout << "Fullscreen " << (fullscreen ? "enabled" : "disabled") << std::endl;
		break;
	}
	case GLFW_KEY_B:
		changeBlue(0.1f);
		break;
	case GLFW_KEY_N:
		changeBlue(-0.1f);
		break;
	}
}

void WindowClass::handleMouseButtonEvent(int button, int action) {
	switch (action) {
	case GLFW_PRESS:
		this->handleMousePress(button, action);
		break;
	default:
		break;
	}
}

void WindowClass::handleMousePress(int button, int action) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		changeGreen(0.1f);
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		changeGreen(-0.1f);
		break;
	}
}

void WindowClass::handleScrollEvent(double x, double y) {
	rgba.r += (y * 0.1f);
	if (rgba.r > 1.0f) rgba.r = 1.0f;
	if (rgba.r < 0.0f) rgba.r = 0.0f;
}

void WindowClass::changeBlue(double val) {
	rgba.b += val;
	if (rgba.b < 0.0f) rgba.b = 0.0f;
	if (rgba.b > 1.0f) rgba.b = 1.0f;
}

void WindowClass::changeGreen(double val) {
	rgba.g += val;
	if (rgba.g < 0.0f) rgba.g = 0.0f;
	if (rgba.g > 1.0f) rgba.g = 1.0f;
}