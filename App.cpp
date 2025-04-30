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

//imgui
#include <imgui.h>
#include <imgui_impl_glfw.h> 
#include <imgui_impl_opengl3.h>

#include "assets.h"
#include "camera.hpp"
#include "LoggerClass.h"
#include "App.h"
#include "FPS.h"
#include "DebugInfo.h"
#include "Mesh.hpp"

App::App()
{
    LoggerClass::info("App has started.");
}

void App::init_imgui(void) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
    ImGui_ImplOpenGL3_Init();
    std::cout << "ImGUI version: " << ImGui::GetVersion() << "\n";
}

void App::init_assets(void) {
    auto vertexShaderPath = std::filesystem::path("./resources/directional.vert");
    auto fragmentShaderPath = std::filesystem::path("./resources/directional.frag");
    auto objectPath = std::filesystem::path("./resources/objects/cube.obj");
    auto texturePath = std::filesystem::path("./resources/textures/box_rgb888.png");
    auto treePath = std::filesystem::path("./resources/objects/Lowpoly_tree_sample.obj");

    //auto camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));

    my_shader = ShaderProgram(vertexShaderPath, fragmentShaderPath);

    window->cam = std::make_unique<Camera>(glm::vec3(550.0f, 273.0f, 580.0f));

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
    Model my_model = Model(objectPath, my_shader, glm::vec3(555.0f, 256.0f, 575.0f));

    GLuint tex = textureInit(texturePath);

    glBindTexture(GL_TEXTURE_2D, tex);

    my_model.tex_ID = tex;
    my_model.addBoxCollider(glm::vec3(555.0f, 256.0f, 575.0f), glm::vec3(0.5f));

    Model my_model2 = Model(objectPath, my_shader, glm::vec3(500.0f, 256.0f, 575.0f));
    my_model2.tex_ID = tex;
    my_model2.addBoxCollider(glm::vec3(500.0f, 256.0f, 575.0f), glm::vec3(0.5f));

    //transparent object
    Model trans_model = Model(objectPath, my_shader, glm::vec3(525.0f, 255.0f, 580.0f));
    trans_model.tex_ID = tex;
    trans_model.transparent = true;
    trans_model.rotate = true;
    trans_model.addSphereCollider(glm::vec3(525.0f, 255.0f, 580.0f), 1.0f);

    Model trans_model2 = Model(objectPath, my_shader, glm::vec3(520.0f, 255.0f, 585.0f));
    trans_model2.tex_ID = tex;
    trans_model2.transparent = true;
    trans_model2.addSphereCollider(glm::vec3(520.0f, 255.0f, 585.0f), 1.0f);

    Model tree_model = Model(treePath, my_shader, glm::vec3(537.0f, 254.0f, 594.0f));
    //obviously not perfect collision (branches dont have collision), but i think good enough for a demo
    tree_model.addBoxCollider(glm::vec3(537.0f, 254.0f, 594.0f), glm::vec3(1.25f, 3.25f, 1.25f));
    tree_model.addSphereCollider(glm::vec3(548.187f, 265.5f, 584.330f), 3.25f);
    tree_model.addSphereCollider(glm::vec3(541.876f, 275.0f, 586.372f), 5.5f);
    tree_model.addSphereCollider(glm::vec3(532.626f, 273.000f, 593.666f), 7.0f);
    tree_model.addSphereCollider(glm::vec3(526.421f, 269.726f, 597.511f), 4.5f);
    tree_model.addSphereCollider(glm::vec3(539.272f, 267.001f, 597.287f), 2.75f);

    Model moving_model = Model(objectPath, my_shader, glm::vec3(510.0f, 256.0f, 575.0f));
    moving_model.tex_ID = tex;
    moving_model.addBoxCollider(glm::vec3(510.0f, 256.0f, 575.0f), glm::vec3(0.5f));
    moving_model.movement = glm::vec3(5.0f, 0.0f, 0.0f);
    moving_model.trackBegin = moving_model.origin;
    moving_model.trackEnd = glm::vec3(545.0f, 256.0f, 575.0f);
    moving_model.moving = true;

    Model moving_model2 = Model(objectPath, my_shader, glm::vec3(520.0f, 254.0f, 575.0f));
    moving_model2.tex_ID = tex;
    moving_model2.addBoxCollider(glm::vec3(520.0f, 254.0f, 575.0f), glm::vec3(0.5f));
    moving_model2.movement = glm::vec3(0.0f, 0.0f, 1.0f);
    moving_model2.trackBegin = moving_model2.origin;
    moving_model2.trackEnd = glm::vec3(520.0f, 254.0f, 580.0f);
    moving_model2.moving = true;
    moving_model2.rotate = true;

    // put model to scene
    //stationary non-transparent models
    scene.insert({ "s_box01", my_model });
    scene.insert({ "s_box02", my_model2 });
    scene.insert({ "s_tree", tree_model });
    //stationary trnsparent models
    scene.insert({ "t_box01", trans_model });
    scene.insert({ "t_box02", trans_model2 });
    //moving non-transparent models
    scene.insert({ "m_box01", moving_model });
    scene.insert({ "m_box02", moving_model2 });
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
        heightMapData = hmap.clone();
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

float App::getHeightMapY(float x, float z) {
    //bilinear interpolation
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int z0 = (int)floor(z);
    int z1 = z0 + 1;

    x0 = glm::clamp(x0, 0, heightMapData.cols - 1);
    x1 = glm::clamp(x1, 0, heightMapData.cols - 1);
    z0 = glm::clamp(z0, 0, heightMapData.rows - 1);
    z1 = glm::clamp(z1, 0, heightMapData.rows - 1);

    float h00 = (float)heightMapData.at<uchar>(cv::Point(x0, z0));
    float h10 = (float)heightMapData.at<uchar>(cv::Point(x1, z0));
    float h01 = (float)heightMapData.at<uchar>(cv::Point(x0, z1));
    float h11 = (float)heightMapData.at<uchar>(cv::Point(x1, z1));

    float tx = x - (float)x0;
    float tz = z - (float)z0;

    float hx0 = glm::mix(h00, h10, tx);
    float hx1 = glm::mix(h01, h11, tx);

    return glm::mix(hx0, hx1, tz);
}

void App::init_pl() {
    PointLight r; PointLight g; PointLight b;

    //positions
    r.position = glm::vec3(541.0f, 254.5f, 594.0f);
    g.position = glm::vec3(537.0f, 254.5f, 598.0f);
    b.position = glm::vec3(535.0f, 254.5f, 592.0f);

    //orbits
    b.orbitMode = OrbitMode::Clockwise;
    g.orbitMode = OrbitMode::CounterClockwise;

    g.orbitCenter = g.position;
    b.orbitCenter = b.position;

    //ambients
    r.ambient = glm::vec3(0.1f, 0.0f, 0.0f);
    g.ambient = glm::vec3(0.0f, 0.1f, 0.0f);
    b.ambient = glm::vec3(0.0f, 0.0f, 0.1f);

    //diffs
    r.diffuse = glm::vec3(1.0f, 0.0f, 0.0f);
    g.diffuse = glm::vec3(0.0f, 1.0f, 0.0f);
    b.diffuse = glm::vec3(0.0f, 0.0f, 1.0f);

    //specs
    r.specular = glm::vec3(1.0f, 0.3f, 0.3f);
    g.specular = glm::vec3(0.3f, 1.0f, 0.3f);
    b.specular = glm::vec3(0.3f, 0.3f, 1.0f);

    //cfloats
    r.constant = g.constant = b.constant = 1.0f;
    r.linear = g.linear = b.linear = 0.09f;
    r.quadratic = g.quadratic = b.quadratic = 0.032f;

    pointLights.push_back(r);
    pointLights.push_back(g);
    pointLights.push_back(b);
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
    bool vsync = false;
    bool fullscreen = false;

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

        if (settings["vsync"].is_boolean()) {
            vsync = settings["vsync"].template get<bool>();
        }

        if (settings["fullscreen"].is_boolean()) {
            fullscreen = settings["fullscreen"].template get<bool>();
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
    std::cout << "VSync: " << (vsync ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Fullscreen: " << (fullscreen ? "Enabled" : "Disabled") << std::endl;

    window = new WindowClass(x, y, name.c_str(), fullscreen, vsync, aa_enabled, aa_level);

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
    init_pl();
    init_imgui();

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
    int fpsCount = 0;
    DebugInfo debug;

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
            //std::cout << "FPS: " << "\t" << fps.getFrames() << std::endl;
            fpsCount = fps.getFrames();
            fps.setFrames(0);
        }

        if (window->show_imgui) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos(ImVec2(10, 10));
            ImGui::SetNextWindowSize(ImVec2(250, 200));

            ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::Text("press RMB to release mouse");
            ImGui::Text("V-Sync: %s", window->getVsync() ? "ON" : "OFF");
            ImGui::Text("FPS: %d", fpsCount);
            ImGui::Text("MSAA changes require restarting");
            ImGui::Text("MSAA: %s", window->getAA() ? "ON" : "OFF");
            if (window->getAA()) {
                ImGui::Text("MSAA level: %d", window->getAAlevel());
            }
            ImGui::Text("hit C to show/hide controls");
            if (window->controls) {
                ImGui::Text("player movement - W, A, S, D");
                ImGui::Text("jump - SPACE");
                ImGui::Text("sprint - LEFT SHIFT");
                ImGui::Text("vsync - V");
                ImGui::Text("fullscreen - F");
                ImGui::Text("toggle flashlight - B");
                ImGui::Text("toggle MSAA - P");
                ImGui::Text("change MSAA level - O");
                ImGui::Text("toggle GUI - H");
                ImGui::Text("change FOV - SCROLL WHEEL");
                ImGui::Text("camera movement - MOUSE MOVEMENT");
                ImGui::Text("exit game - ESCAPE");
            }
            ImGui::End();
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        
        window->cam->processInput(window->getWindow(), deltaTime);

        //get hmap Y coord with added height ("eye level")
        float hmapY = getHeightMapY(window->cam->Position.x, window->cam->Position.z) + window->cam->camRadius;
        window->cam->clampY(hmapY);

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

        for (int i = 0; i < pointLights.size(); i++) {
            pointLights[i].orbit(deltaTime);

            glm::vec3 plViewPos = glm::vec3(window->cam->getViewMatrix() * glm::vec4(pointLights[i].position, 1.0));

            my_shader.setUniform("pointLights[" + std::to_string(i) + "].Pposition", plViewPos);
            my_shader.setUniform("pointLights[" + std::to_string(i) + "].Pambient", pointLights[i].ambient);
            my_shader.setUniform("pointLights[" + std::to_string(i) + "].Pdiffuse", pointLights[i].diffuse);
            my_shader.setUniform("pointLights[" + std::to_string(i) + "].Pspecular", pointLights[i].specular);

            my_shader.setUniform("pointLights[" + std::to_string(i) + "].Pconstant", pointLights[i].constant);
            my_shader.setUniform("pointLights[" + std::to_string(i) + "].Plinear", pointLights[i].linear);
            my_shader.setUniform("pointLights[" + std::to_string(i) + "].Pquadratic", pointLights[i].quadratic);
        }

        heightMap.draw(glm::vec3(0.0), glm::vec3(0.0));

        //my_shader.setUniform("u_diffuse_color", glm::vec4(1.0f));

        my_shader.setUniform("tex0", 0);

        for (auto& [name, model] : scene) {
            //if (model.moving) {
            //    std::cout << model.trackBegin << std::endl;
            //    std::cout << model.trackEnd << std::endl;
            //}
            for (const auto& c : model.colliders) {
                glm::vec3 cPos = model.getColliderWorldPosition(c);
                //std::cout << cPos << std::endl;

                if (c.type == Collider::Sphere) {
                    glm::vec3 collisionNormal = glm::normalize(window->cam->getPosition() - c.position);
                    float distance = glm::length(window->cam->getPosition() - c.position);
                    float penetration = (window->cam->camRadius + c.radius) - distance;

                    if (penetration > 0.0f) {
                        window->cam->Position += collisionNormal * penetration;
                    }
                    
                }
                else if (c.type == Collider::Box) {
                    glm::vec3 relCenter = glm::transpose(c.rotationMatrix) * (window->cam->getPosition() - c.position);
                    glm::vec3 collisionNormal = glm::normalize(window->cam->getPosition() - cPos);
                    bool penCheck = true;

                    glm::vec3 closestPoint = glm::clamp(relCenter, -c.halfSize, c.halfSize);
                    glm::vec3 delta = relCenter-closestPoint;

                    if (abs(window->cam->getPosition().y - (c.position.y + c.halfSize.y + window->cam->camRadius)) < 0.15f) {
                        if (relCenter.x >= -c.halfSize.x && relCenter.x <= c.halfSize.x &&
                            relCenter.z >= -c.halfSize.z && relCenter.z <= c.halfSize.z) {

                            window->cam->clampY(c.position.y + c.halfSize.y + window->cam->camRadius);
                            window->cam->Position += model.moveDelta;
                            penCheck = false;
                        }
                    }

                    float distance = glm::length(delta);
                    float pen = window->cam->camRadius - distance;
                    
                    if (pen > 0.05f && penCheck) {
                        glm::vec3 pushDir;
                        float pushAmount = 0.0f;
                        if (pen > 0.4f) {
                            float penX = window->cam->camRadius - abs(delta.x);
                            float penY = window->cam->camRadius - abs(delta.y);
                            float penZ = window->cam->camRadius - abs(delta.z);

                            if (penX < penY && penX < penZ) {
                                pushDir = glm::vec3((delta.x > 0.0f ? 1.0f : -1.0f), 0.0f, 0.0f);
                                pushAmount = (abs(delta.x) - c.halfSize.x) + window->cam->camRadius + 0.01f;
                            }
                            else if (penY < penX && penY < penZ) {
                                pushDir = glm::vec3(0.0f, (delta.y > 0.0f ? 1.0f : -1.0f), 0.0f);
                                pushAmount = (abs(delta.y) - c.halfSize.y) + window->cam->camRadius + 0.01f;
                            }
                            else {
                                pushDir = glm::vec3(0.0f, 0.0f, (delta.z > 0.0f ? 1.0f : -1.0f));
                                pushAmount = (abs(delta.z) - c.halfSize.z) + window->cam->camRadius + 0.01f;
                            }

                            window->cam->Position += c.rotationMatrix * (pushDir * pushAmount);

                        }
                        else {
                            float penX = window->cam->camRadius - abs(delta.x);
                            float penY = window->cam->camRadius - abs(delta.y);
                            float penZ = window->cam->camRadius - abs(delta.z);

                            float extra = 0.1f;

                            if (penX < penY && penX < penZ) {
                                window->cam->Position.x += (delta.x > 0.0f ? 1.0f : -1.0f) * (penX + extra);
                            }
                            else if (penY < penX && penY < penZ) {
                                window->cam->Position.y += (delta.y > 0.0f ? 1.0f : -1.0f) * (penY + extra);
                            }
                            else {
                                window->cam->Position.z += (delta.z > 0.0f ? 1.0f : -1.0f) * (penZ + extra);
                            }
                        }
                    }
                }
            }

            if (!model.transparent) {
                model.draw(glm::vec3(0.0), glm::vec3(0.0), glm::mat4(1.0f), window, deltaTime);
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
            p->draw(glm::vec3(0.0), glm::vec3(0.0), glm::mat4(1.0f), window, deltaTime);
        }

        glDepthMask(GL_TRUE);

        if (window->show_imgui) {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(window->getWindow());
        glfwPollEvents();
    }

    updateConfig();

    return 0;
}

void App::updateConfig() {
    std::ifstream inFile("app_settings.json");
    nlohmann::json config;
    if (inFile.is_open()) {
        inFile >> config;
        int w;
        int h;
        glfwGetWindowSize(window->getWindow(), &w, &h);
        config["default_resolution"]["x"] = w;
        config["default_resolution"]["y"] = h;

        config["antialiasing"]["enabled"] = window->getAA();
        config["antialiasing"]["level"] = window->getAAlevel();

        config["vsync"] = window->getVsync();
        config["fullscreen"] = window->getFullscreen();
    }
    else {
        std::cerr << "Could not open config file.\n";
    }

    std::ofstream outFile("app_settings.json");
    if (outFile.is_open()) {
        outFile << config.dump(2);
    }
    else {
        std::cerr << "Could not write to config file.\n";
    }
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
