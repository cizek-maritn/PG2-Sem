//testing
#define GLM_ENABLE_EXPERIMENTAL

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
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "assets.h"
#include "camera.hpp"
#include "LoggerClass.h"
#include "App.h"
#include "FPS.h"
#include "DebugInfo.h"
#include "Mesh.hpp"
#include "assets.h"

App::App()
{
    LoggerClass::info("App has started.");
}

void App::init_assets(void) {
    auto vertexShaderPath = std::filesystem::path("./resources/directional.vert");
    auto fragmentShaderPath = std::filesystem::path("./resources/directional.frag");
    auto objectPath = std::filesystem::path("./resources/objects/cube.obj");
    auto texturePath = std::filesystem::path("./resources/textures/box_rgb888.png");
    auto treePath = std::filesystem::path("./resources/objects/Lowpoly_tree_sample.obj");

    //auto camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));

    my_shader = ShaderProgram(vertexShaderPath, fragmentShaderPath);

    window->cam = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 1.0f));

    my_shader.activate();

    my_shader.setUniform("p_m", window->cam->getProjMatrix());
    my_shader.setUniform("v_m", window->cam->getViewMatrix());
    my_shader.setUniform("m_m", glm::mat4(1.0f));

    my_shader.setUniform("light_direction", glm::vec3(10.0f));

    my_shader.setUniform("ambient_intensity", glm::vec3(1.0f));
    my_shader.setUniform("diffuse_intensity", glm::vec3(1.0f));
    my_shader.setUniform("specular_intensity", glm::vec3(1.0f));

    my_shader.setUniform("ambient_material", glm::vec3(1.0f));
    my_shader.setUniform("diffuse_material", glm::vec3(1.0f));
    my_shader.setUniform("specular_material", glm::vec3(1.0f));
    my_shader.setUniform("specular_shininess", 32.0f);

    my_shader.setUniform("useSpotlight", 0);

    my_shader.setUniform("alpha", 1.0f);

    my_shader.setUniform("tex0", 0);

    // model: load model file, assign shader used to draw a model
    Model my_model = Model(objectPath, my_shader, glm::vec3(0.0f));

    GLuint tex = textureInit(texturePath);

    glBindTexture(GL_TEXTURE_2D, tex);

    my_model.tex_ID = tex;

    //transparent object
    Model trans_model = Model(objectPath, my_shader, glm::vec3(2.0f));
    trans_model.tex_ID = tex;
    trans_model.transparent = true;

    Model tree_model = Model(treePath, my_shader, glm::vec3(537.0f, 254.0f, 594.0f));
    //Model tree_model = Model(treePath, my_shader, glm::vec3(4.0f, 4.0f, 4.0f));

    // put model to scene
    scene.insert({ "my_first_object", my_model });
    scene.insert({ "trans_object", trans_model });
    scene.insert({ "tree", tree_model });
}

GLuint App::textureInit(const std::filesystem::path& file_name)
{
    cv::Mat image = cv::imread(file_name.string(), cv::IMREAD_UNCHANGED);  // Read with (potential) Alpha
    if (image.empty()) {
        throw std::runtime_error("No texture in file: " + file_name.string());
    }
    //std::cout << "Image Loaded: " << image.cols << "x" << image.rows << " Channels: " << image.channels() << std::endl;
    // or print warning, and generate synthetic image with checkerboard pattern 
    // using OpenCV and use as a texture replacement 

    GLuint texture = gen_tex(image);
    //std::cout << texture << std::endl;
    return texture;
}

GLuint App::gen_tex(cv::Mat& image)
{
    GLuint ID = 0;

    if (image.empty()) {
        throw std::runtime_error("Image empty?\n");
    }

    // Generates an OpenGL texture object
    glCreateTextures(GL_TEXTURE_2D, 1, &ID);

    switch (image.channels()) {
    case 3:
        // Create and clear space for data - immutable format
        glTextureStorage2D(ID, 1, GL_RGB8, image.cols, image.rows);
        // Assigns the image to the OpenGL Texture object
        glTextureSubImage2D(ID, 0, 0, 0, image.cols, image.rows, GL_BGR, GL_UNSIGNED_BYTE, image.data);
        break;
    case 4:
        glTextureStorage2D(ID, 1, GL_RGBA8, image.cols, image.rows);
        glTextureSubImage2D(ID, 0, 0, 0, image.cols, image.rows, GL_BGRA, GL_UNSIGNED_BYTE, image.data);
        break;
    default:
        throw std::runtime_error("unsupported channel cnt. in texture:" + std::to_string(image.channels()));
    }   

    // Configures the type of algorithm that is used to make the image smaller or bigger
    // nearest neighbor - ugly & fast 
    //glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
    //glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // bilinear - nicer & slower
    glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // MIPMAP filtering + automatic MIPMAP generation - nicest, needs more memory. Notice: MIPMAP is only for image minifying.
    //glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // bilinear magnifying
    //glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // trilinear minifying
    glGenerateTextureMipmap(ID);  //Generate mipmaps now.

    // Configures the way the texture repeats
    glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return ID;
}

void App::init_hm(void)
{
    // height map
    {
        std::filesystem::path hm_file("./resources/textures/heights.png");
        cv::Mat hmap = cv::imread(hm_file.string(), cv::IMREAD_GRAYSCALE);

        if (hmap.empty())
            throw std::runtime_error("ERR: Height map empty? File: " + hm_file.string());

        heightMap = GenHeightMap(hmap, 10); //image, step size
        std::cout << "Note: height map vertices: " << heightMap.vertices.size() << std::endl;
    }
}

Mesh App::GenHeightMap(const cv::Mat& hmap, const unsigned int mesh_step_size)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    glm::vec3 v;
    glm::vec4 c;

    std::cout << "Note: heightmap size:" << hmap.size << ", channels: " << hmap.channels() << std::endl;

    if (hmap.channels() != 1) {
        std::cerr << "WARN: requested 1 channel, got: " << hmap.channels() << std::endl;
    }

    // Create heightmap mesh from TRIANGLES in XZ plane, Y is UP (right hand rule)
    //
    //   3-----2
    //   |    /|
    //   |  /  |
    //   |/    |
    //   0-----1
    //
    //   012,023
    //

    for (unsigned int x_coord = 0; x_coord < (hmap.cols - mesh_step_size); x_coord += mesh_step_size)
    {
        for (unsigned int z_coord = 0; z_coord < (hmap.rows - mesh_step_size); z_coord += mesh_step_size)
        {
            // Get The (X, Y, Z) Value For The Bottom Left Vertex = 0
            glm::vec3 p0(x_coord, hmap.at<uchar>(cv::Point(x_coord, z_coord)), z_coord);
            // Get The (X, Y, Z) Value For The Bottom Right Vertex = 1
            glm::vec3 p1(x_coord + mesh_step_size, hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord)), z_coord);
            // Get The (X, Y, Z) Value For The Top Right Vertex = 2
            glm::vec3 p2(x_coord + mesh_step_size, hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord + mesh_step_size)), z_coord + mesh_step_size);
            // Get The (X, Y, Z) Value For The Top Left Vertex = 3
            glm::vec3 p3(x_coord, hmap.at<uchar>(cv::Point(x_coord, z_coord + mesh_step_size)), z_coord + mesh_step_size);

            // Get max normalized height for tile, set texture accordingly
            // Grayscale image returns 0..256, normalize to 0.0f..1.0f by dividing by 256
            float max_h = std::max(hmap.at<uchar>(cv::Point(x_coord, z_coord)) / 256.0f,
                std::max(hmap.at<uchar>(cv::Point(x_coord, z_coord + mesh_step_size)) / 256.0f,
                    std::max(hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord + mesh_step_size)) / 256.0f,
                        hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord)) / 256.0f
                    )));

            // Get texture coords in vertices, bottom left of geometry == bottom left of texture
            glm::vec2 tc0 = get_subtex_by_height(max_h);
            glm::vec2 tc1 = tc0 + glm::vec2(1.0f / 16, 0.0f);       //add offset for bottom right corner
            glm::vec2 tc2 = tc0 + glm::vec2(1.0f / 16, 1.0f / 16);  //add offset for top right corner
            glm::vec2 tc3 = tc0 + glm::vec2(0.0f, 1.0f / 16);       //add offset for bottom leftcorner

            // normals for both triangles, CCW
            glm::vec3 n1 = -glm::normalize(glm::cross(p1 - p0, p2 - p0)); // for p1
            glm::vec3 n2 = -glm::normalize(glm::cross(p2 - p0, p3 - p0)); // for p3
            glm::vec3 navg = glm::normalize(n1 + n2);                 // average for p0, p2 - common

            //place vertices and ST to mesh
            vertices.emplace_back(Vertex(p0, navg, tc0));
            vertices.emplace_back(Vertex(p1, n1, tc1));
            vertices.emplace_back(Vertex(p2, navg, tc2));
            vertices.emplace_back(Vertex(p3, n2, tc3));

            // place indices
            GLuint baseIndex = static_cast<GLuint>(vertices.size()-4);
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + 2);
            indices.push_back(baseIndex + 1);
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + 3);
            indices.push_back(baseIndex + 2);
        }
    }

    /*auto vertexShaderPath = std::filesystem::path("./resources/directional.vert");
    auto fragmentShaderPath = std::filesystem::path("./resources/directional.frag");
    hmapShader = ShaderProgram(vertexShaderPath, fragmentShaderPath);

    hmapShader.activate();

    hmapShader.setUniform("p_m", window->cam->getProjMatrix());
    hmapShader.setUniform("v_m", window->cam->getViewMatrix());
    hmapShader.setUniform("m_m", glm::mat4(1.0f));

    hmapShader.setUniform("light_position", glm::vec3(10.0f));

    hmapShader.setUniform("ambient_intensity", glm::vec3(1.0f));
    hmapShader.setUniform("diffuse_intensity", glm::vec3(1.0f));
    hmapShader.setUniform("specular_intensity", glm::vec3(1.0f));

    hmapShader.setUniform("ambient_material", glm::vec3(1.0f));
    hmapShader.setUniform("diffuse_material", glm::vec3(1.0f));
    hmapShader.setUniform("specular_material", glm::vec3(1.0f));
    hmapShader.setUniform("specular_shininess", 32.0f);*/

    auto texturePath = std::filesystem::path("./resources/textures/tex_256.png");
    GLuint tex = textureInit(texturePath);
    hmapTex = tex;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    int i = 0;
    glBindTextureUnit(i, tex);

    GLenum primitive_type = GL_TRIANGLES;

    my_shader.setUniform("tex0", i);

    return Mesh(primitive_type, my_shader, vertices, indices, glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0),false, i);
}

glm::vec2 App::get_subtex_st(const int x, const int y)
{
    return glm::vec2(x * 1.0f / 16, y * 1.0f / 16);
}

// choose subtexture based on height
glm::vec2 App::get_subtex_by_height(float height)
{
    if (height > 0.9)
        return get_subtex_st(2, 4); //snow
    else if (height > 0.8)
        return get_subtex_st(3, 4); //ice
    else if (height > 0.5)
        return get_subtex_st(2, 2); //rock
    else if (height > 0.3)
        return get_subtex_st(2, 0); //soil
    else
        return get_subtex_st(0, 0); //grass
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
    bool aa_enabled = false;
    int aa_level = 4;

    std::ifstream sett_file("app_settings.json");
    if (!sett_file.is_open()) {
        std::cerr << "Error: Could not open settings file. Using default settings.\n";
    }
    else {
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

        if (settings["antialiasing"]["enabled"].is_boolean()) {
            aa_enabled = settings["antialiasing"]["enabled"].template get<bool>();
        }

        if (settings["antialiasing"]["level"].is_number_integer()) {
            aa_level = settings["antialiasing"]["level"].template get<int>();
        }
    }

    if (aa_enabled) {
        if (aa_level <= 1) {
            std::cerr << "Warning: Antialiasing enabled but level is 1 or lower. Disabling AA.\n";
            aa_enabled = false;
        }
        else if (aa_level > 8) {
            std::cerr << "Warning: Antialiasing level too high (>8). Clamping to 8.\n";
            aa_level = 8;
        }
    }

    std::cout << "app name: " << name << std::endl;
    std::cout << "[x,y] = [" << x << ',' << y << "]\n";
    std::cout << "Antialiasing: " << (aa_enabled ? "Enabled" : "Disabled") << " (Level: " << aa_level << ")\n";

    window = new WindowClass(x, y, name.c_str(), NULL, NULL, aa_enabled, aa_level);

    glewInit();
    wglewInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 11);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    if (!GLEW_ARB_direct_state_access)
        throw std::runtime_error("No DSA :-(");

    
    init_assets();
    init_hm();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

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

    LoggerClass::debug("Backface culling enabled (CCW).");
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

        std::vector<Model*> transparent;    // temporary, vector of pointers to transparent objects
        transparent.reserve(scene.size());  // reserve size for all objects to avoid reallocation

        /*glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));*/

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        my_shader.activate();

        my_shader.setUniform("p_m", window->cam->getProjMatrix());
        my_shader.setUniform("v_m", window->cam->getViewMatrix());
        my_shader.setUniform("m_m", glm::mat4(1.0f));

        glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -0.25f, -1.0f));
        lightDir = glm::rotate(lightDir, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        my_shader.setUniform("light_direction", lightDir);

        my_shader.setUniform("ambient_material", glm::vec3(1.0f));
        my_shader.setUniform("diffuse_material", glm::vec3(0.8f));
        my_shader.setUniform("specular_material", glm::vec3(1.0f));

        my_shader.setUniform("ambient_intensity", glm::vec3(0.2f));
        my_shader.setUniform("diffuse_intensity", glm::vec3(0.8f));
        my_shader.setUniform("specular_intensity", glm::vec3(0.5f));
        my_shader.setUniform("specular_shinines", 512.0f);
        my_shader.setUniform("alpha", 1.0f);

        my_shader.setUniform("useSpotlight", window->useSpotlight);
        my_shader.setUniform("useTexture", 1);

        glm::vec3 spotPosView = glm::vec3(window->cam->getViewMatrix() * glm::vec4(window->cam->getPosition(), 1.0));
        glm::vec3 spotDirView = glm::mat3(window->cam->getViewMatrix()) * window->cam->Front;

        my_shader.setUniform("spotlight_position", spotPosView);
        my_shader.setUniform("spotlight_direction", spotDirView);

        my_shader.setUniform("spotlight_cutOff", glm::cos(glm::radians(20.0f)));
        my_shader.setUniform("spotlight_outerCutOff", glm::cos(glm::radians(35.0f)));
        my_shader.setUniform("spotlight_constant", 1.0f);
        my_shader.setUniform("spotlight_linear", 0.09f);
        my_shader.setUniform("spotlight_quadratic", 0.032f);

        glActiveTexture(GL_TEXTURE0);
        int i = 0;
        glBindTextureUnit(i, hmapTex);
        my_shader.setUniform("tex0", i);

        heightMap.draw(glm::vec3(0.0), glm::vec3(0.0));

        //my_shader.setUniform("u_diffuse_color", glm::vec4(1.0f));

        my_shader.setUniform("tex0", 0);

        for (auto& [name, model] : scene) {
            if (!model.transparent) {
                model.draw(glm::vec3(0.0), glm::vec3(0.0), glm::mat4(1.0f), window);
            }
            else {
                transparent.emplace_back(&model); // save pointer for painters algorithm
            }
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        std::sort(transparent.begin(), transparent.end(), [&](Model const* a, Model const* b) {
            glm::vec3 translation_a = glm::vec3(a->model_matrix[3]);  // get 3 values from last column of model matrix = translation
            glm::vec3 translation_b = glm::vec3(b->model_matrix[3]);  // dtto for model B
            return glm::distance(window->cam->Position, translation_a) < glm::distance(window->cam->Position, translation_b); // sort by distance from camera
            });

        my_shader.setUniform("alpha", 0.5f);

        for (auto p : transparent) {
            p->draw(glm::vec3(0.0), glm::vec3(0.0), glm::mat4(1.0f), window);
        }

        glDepthMask(GL_TRUE);

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
    for (auto& [name, model] : scene) {
        model.clear();
    }
    cv::destroyAllWindows();
    std::cout << "Bye...\n";
}
