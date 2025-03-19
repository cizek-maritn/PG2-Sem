//testing
#include <iostream>
#include <fstream>
#include <sstream>
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
#include "camera.hpp"
#include "LoggerClass.h"
#include "App.h"
#include "FPS.h"
#include "DebugInfo.h"

App::App()
{
    LoggerClass::info("App has started.");
}

void App::init_assets(void) {
    auto vertexShaderPath = std::filesystem::path("./resources/basic_core.vert");
    auto fragmentShaderPath = std::filesystem::path("./resources/basic_core.frag");
    auto objectPath = std::filesystem::path("./resources/objects/triangle.obj");

    //auto camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));

    my_shader = ShaderProgram(vertexShaderPath, fragmentShaderPath);

    window->cam = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 1.0f));

    my_shader.setUniform("uP_m", window->cam->getProjMatrix());
    my_shader.setUniform("uV_m", window->cam->getViewMatrix());
    my_shader.activate();

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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 11);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!GLEW_ARB_direct_state_access)
        throw std::runtime_error("No DSA :-(");

    init_assets();

    return true;
}

void App::report(void) {
    GLint extensionCount;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

    std::ostringstream vendor, renderer, version, extensions;

    vendor << "Vendor: " << glGetString(GL_VENDOR);
    renderer << "Renderer: " << glGetString(GL_RENDERER);
    version << "Version: " << glGetString(GL_VERSION);
    extensions << "Extensions: " << extensionCount;

    LoggerClass::debug(vendor.str());
    LoggerClass::debug(renderer.str());
    LoggerClass::debug(version.str());
    LoggerClass::debug(extensions.str());

    /*glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);*/
    glEnable(GL_DEPTH_TEST);

    LoggerClass::debug("Backface culling enabled (CW).");
}

int App::run(void)
{
    FPS fps;
    DebugInfo debug;
    window->rgba = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    LoggerClass::debug("Debug output: " + (debug.available ? std::string("yes") : std::string("no")));

    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;

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

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        window->cam->processInput(window->getWindow(), deltaTime);

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

        my_shader.setUniform("uniform_Color", window->rgba);
        my_shader.setUniform("uP_m", window->cam->getProjMatrix());
        my_shader.setUniform("uV_m", window->cam->getViewMatrix());
        my_shader.setUniform("uM_m", trans);
        my_shader.activate();

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
