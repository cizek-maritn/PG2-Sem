// C++
// include anywhere, in any order
#include <iostream>

// OpenCV (does not depend on GL)
#include <opencv2\opencv.hpp>

// OpenGL Extension Wrangler: allow all multiplatform GL functions
#include <GL/glew.h> 
// WGLEW = Windows GL Extension Wrangler (change for different platform) 
// platform specific functions (in this case Windows)
#include <GL/wglew.h> 

// GLFW toolkit
// Uses GL calls to open GL context, i.e. GLEW __MUST__ be first.
#include <GLFW/glfw3.h>

#include "App.h"


// global variables
App app;

int main()
{
	bool initialized = app.init();
	if (!initialized) return 1;
	app.report();
	return app.run();
}


