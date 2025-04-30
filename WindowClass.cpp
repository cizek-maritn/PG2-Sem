#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "WindowClass.h"
#include "LoggerClass.h"
#include "camera.hpp"
//#include "App.h"

float WindowClass::lastX = 0.0f;
float WindowClass::lastY = 0.0f;
bool WindowClass::mouseMove = true;
std::unique_ptr<Camera> WindowClass::cam = nullptr;
bool WindowClass::cursor = false;

WindowClass::WindowClass(int w, int h, const char* name, bool fullscreen, bool vsync, bool aa, int aa_level) : fullscreen(fullscreen), vsync(vsync), aa_enabled(aa), aa_level(aa_level) {
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

bool WindowClass::getAA() const {
	return aa_enabled;
}

int WindowClass::getAAlevel() const {
	return aa_level;
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
	if (!WindowClass::cursor) {
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
		if (this->useSpotlight) this->useSpotlight = 0;
		else this->useSpotlight = 1;
		//std::cout << this->useSpotlight << std::endl;
		break;
	case GLFW_KEY_P:
		aa_enabled = !aa_enabled;
		break;
	case GLFW_KEY_O:
		if (aa_enabled) {
			if (aa_level == 2) aa_level = 4;
			else if (aa_level == 4) aa_level = 8;
			else aa_level = 2;
		}
		break;
	case GLFW_KEY_C:
		controls = !controls;
		break;
	case GLFW_KEY_H:
		show_imgui = !show_imgui;
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
		//std::cout << cam->Position << std::endl;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (cursor) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cursor = !cursor;
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			lastX = static_cast<float>(xpos);
			lastY = static_cast<float>(ypos);
			mouseMove = true;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cursor = !cursor;
		}
		break;
	}
}

void WindowClass::handleScrollEvent(double x, double y) {
	this->cam->FOV -= 5 * y;
	if (this->cam->FOV > 90.0f) this->cam->FOV = 90.0f;
	if (this->cam->FOV < 30.0f) this->cam->FOV = 30.0f;
}