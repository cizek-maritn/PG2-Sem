#include "FPS.h"
#include <GLFW/glfw3.h>

FPS::FPS() {
	frames = 0;
	prevTime = glfwGetTime();
	currentTime = glfwGetTime();
}

bool FPS::secondPassed() {
	currentTime = glfwGetTime();
	frames++;
	return currentTime - prevTime >= 1.0;
}

int FPS::getFrames() {
	return frames;
}

void FPS::setFrames(int n) {
	frames = n;
	prevTime = glfwGetTime();
}