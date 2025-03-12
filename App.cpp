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
    auto vertexShaderPath = std::filesystem::path("./resources/basic3.vert");
    auto fragmentShaderPath = std::filesystem::path("./resources/basic3.frag");
    auto objectPath = std::filesystem::path("./resources/objects/triangle.obj");

    my_shader = ShaderProgram(vertexShaderPath, fragmentShaderPath);

    // model: load model file, assign shader used to draw a model
    Model my_model = Model(objectPath, my_shader);

    // put model to scene
    scene.insert({ "my_first_object", my_model });
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
    window->rgba = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    std::cout << "Debug output: " << "\t" << (debug.available ? "yes" : "no") << std::endl;

    //glUseProgram(shader_prog_ID);

    // Get uniform location in GPU program. This will not change, so it can be moved out of the game loop.
    /*GLint uniform_color_location = glGetUniformLocation(shader_prog_ID, "uniform_Color");
    if (uniform_color_location == -1) {
        std::cerr << "Uniform location is not found in active shader program. Did you forget to activate it?\n";
    }*/

    while (!glfwWindowShouldClose(window->getWindow())) {
        if (fps.secondPassed()) {
            std::cout << "FPS: " << "\t" << fps.getFrames() << std::endl;
            fps.setFrames(0);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        my_shader.activate();
        my_shader.setUniform("ucolor", window->rgba);

        //glUniform4f(uniform_color_location, window->r, window->g, window->b, window->a);

        ////bind 3d object data
        //glBindVertexArray(VAO_ID);

        //// draw all VAO data
        //glDrawArrays(GL_TRIANGLES, 0, triangle_vertices.size());

        for (auto& [name, model] : scene) {
            model.draw(glm::vec3(0.0), glm::vec3(0.0));
        }

        glfwSwapBuffers(window->getWindow());
        glfwPollEvents();
    }
    return 0;
}

App::~App()
{
    // clean-up
    /*glDeleteProgram(shader_prog_ID);
    glDeleteBuffers(1, &VBO_ID);
    glDeleteVertexArrays(1, &VAO_ID);*/
    my_shader.clear();
    cv::destroyAllWindows();
    std::cout << "Bye...\n";
}
