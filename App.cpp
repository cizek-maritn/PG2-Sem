//testing
#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/opencv.hpp>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>

// OpenGL math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "assets.h"
#include "App.h"
#include "FPS.h"
#include "DebugInfo.h"

App::App()
{
    std::cout << "starting" << std::endl;
}

void App::init_assets(void) {
    //
    // Initialize pipeline: compile, link and use shaders
    //

    //SHADERS - define & compile & link
    const char* vertex_shader =
        "#version 460 core\n"
        "in vec3 attribute_Position;"
        "void main() {"
        "  gl_Position = vec4(attribute_Position, 1.0);"
        "}";

    const char* fragment_shader =
        "#version 460 core\n"
        "uniform vec4 uniform_Color;"
        "out vec4 FragColor;"
        "void main() {"
        "  FragColor = uniform_Color;"
        "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    shader_prog_ID = glCreateProgram();
    glAttachShader(shader_prog_ID, fs);
    glAttachShader(shader_prog_ID, vs);
    glLinkProgram(shader_prog_ID);

    //now we can delete shader parts (they can be reused, if you have more shaders)
    //the final shader program already linked and stored separately
    glDetachShader(shader_prog_ID, fs);
    glDetachShader(shader_prog_ID, vs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // 
    // Create and load data into GPU using OpenGL DSA (Direct State Access)
    //

    // Create VAO + data description (just envelope, or container...)
    glCreateVertexArrays(1, &VAO_ID);

    GLint position_attrib_location = glGetAttribLocation(shader_prog_ID, "attribute_Position");

    glEnableVertexArrayAttrib(VAO_ID, position_attrib_location);
    glVertexArrayAttribFormat(VAO_ID, position_attrib_location, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, position));
    glVertexArrayAttribBinding(VAO_ID, position_attrib_location, 0); // (GLuint vaobj, GLuint attribindex, GLuint bindingindex)

    // Create and fill data
    glCreateBuffers(1, &VBO_ID);
    glNamedBufferData(VBO_ID, triangle_vertices.size() * sizeof(vertex), triangle_vertices.data(), GL_STATIC_DRAW);

    // Connect together
    glVertexArrayVertexBuffer(VAO_ID, 0, VBO_ID, 0, sizeof(vertex)); // (GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)
}

bool App::init()
{
    if (!glfwInit()) {
        return false;
    }

    //default values
    std::string name = "OpenGL Context";
    int x = 800;
    int y = 600;

    std::ifstream sett_file("app_settings.json");
    nlohmann::json settings = nlohmann::json::parse(sett_file);

    name = settings["appname"];

    // getting value - safely
    if (settings["default_resolution"]["x"].is_number_integer()) {
        // key found and value is proper type, use safe conversion            
        x = settings["default_resolution"]["x"].template get<int>();
    }

    if (settings["default_resolution"]["y"].is_number_integer()) {
        // key found and value is proper type, use safe conversion            
        y = settings["default_resolution"]["y"].template get<int>();
    }

    std::cout << "app name: " << name << std::endl;
    std::cout << "[x,y] = [" << x << ',' << y << "]\n";

    window = new WindowClass(x, y, name.c_str(), NULL, NULL);

    glewInit();
    wglewInit();

    if (!GLEW_ARB_direct_state_access)
        throw std::runtime_error("No DSA :-(");

    init_assets();

    return true;
}

void App::report(void) {
    GLint extensionCount;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

    std::cout << "OpenGL Info: " << std::endl;
    std::cout << "Vendor: " << "\t" << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << "\t" << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << "\t" << glGetString(GL_VERSION) << std::endl;
    std::cout << "Extensions: " << "\t" << extensionCount << std::endl;
}

int App::run(void)
{
    FPS fps;
    DebugInfo debug;
    window->r = window->a = 1.0f;
    window->g = window->b = 0.0f;

    std::cout << "Debug output: " << "\t" << (debug.available ? "yes" : "no") << std::endl;

    glUseProgram(shader_prog_ID);

    // Get uniform location in GPU program. This will not change, so it can be moved out of the game loop.
    GLint uniform_color_location = glGetUniformLocation(shader_prog_ID, "uniform_Color");
    if (uniform_color_location == -1) {
        std::cerr << "Uniform location is not found in active shader program. Did you forget to activate it?\n";
    }

    while (!glfwWindowShouldClose(window->getWindow())) {
        if (fps.secondPassed()) {
            std::cout << "FPS: " << "\t" << fps.getFrames() << std::endl;
            fps.setFrames(0);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniform4f(uniform_color_location, window->r, window->g, window->b, window->a);

        //bind 3d object data
        glBindVertexArray(VAO_ID);

        // draw all VAO data
        glDrawArrays(GL_TRIANGLES, 0, triangle_vertices.size());

        glfwSwapBuffers(window->getWindow());
        glfwPollEvents();
    }
    return 0;
}

App::~App()
{
    // clean-up
    glDeleteProgram(shader_prog_ID);
    glDeleteBuffers(1, &VBO_ID);
    glDeleteVertexArrays(1, &VAO_ID);
    cv::destroyAllWindows();
    std::cout << "Bye...\n";
}
