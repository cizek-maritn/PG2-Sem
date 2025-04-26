#pragma once

#include <filesystem>
#include <string>
#include <vector> 
#include <glm/glm.hpp> 

#include "assets.h"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"
#include "OBJloader.hpp"

class Model {
public:
    std::vector<Mesh> meshes;
    std::string name;
    glm::vec3 origin{};
    glm::vec3 orientation{};
    ShaderProgram shader{};
    GLuint tex_ID{};
    bool transparent{ false };
    glm::mat4 model_matrix;
    
    Model(const std::filesystem::path & filename, ShaderProgram shader, glm::vec3 origin) {
        // load mesh (all meshes) of the model, (in the future: load material of each mesh, load textures...)
        // TODO: call LoadOBJFile, LoadMTLFile (if exist), process data, create mesh and set its properties
        //    notice: you can load multiple meshes and place them to proper positions, 
        //            multiple textures (with reusing) etc. to construct single complicated Model  
        this->origin = origin;
        this->orientation = glm::vec3(0.0f);
        this->meshes = std::vector<Mesh>{};
        this->name = filename.string();
        this->shader = shader;

        auto filetype = filename.extension().string();
        if (filetype == ".obj") {
            auto loader = OBJLoader(filename);
            meshes=loader.getMesh(shader, this->origin, this->orientation);
        }
    }

    // update position etc. based on running time
    void update(const float delta_t) {
        // origin += glm::vec3(3,0,0) * delta_t; // s = s0 + v*dt
    }
    
    void draw(glm::vec3 const & offset = glm::vec3(0.0), glm::vec3 const & rotation = glm::vec3(0.0f), glm::mat4 const& trans = glm::mat4(1.0f), WindowClass* window = nullptr) {
        /*GLint activeTexture;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
        std::cout << "Active Texture Unit: " << activeTexture << ", Bound Texture ID: " << tex_ID << std::endl;*/

        shader.activate();

        // Compute the model matrix correctly
        this->model_matrix = glm::mat4(1.0f);
        this->model_matrix = glm::translate(this->model_matrix, origin + offset);  // Apply object origin and additional offset
        this->model_matrix = glm::rotate(this->model_matrix, glm::radians(orientation.x + rotation.x), glm::vec3(1.0, 0.0, 0.0));
        this->model_matrix = glm::rotate(this->model_matrix, glm::radians(orientation.y + rotation.y), glm::vec3(0.0, 1.0, 0.0));
        this->model_matrix = glm::rotate(this->model_matrix, glm::radians(orientation.z + rotation.z), glm::vec3(0.0, 0.0, 1.0));

        // Step 2: Apply local rotation (rotates around its own Y-axis)
        this->model_matrix = glm::rotate(this->model_matrix, glm::radians(orientation.y + rotation.y) + (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));

        // Combine with any additional transformation matrix (`trans`)
        this->model_matrix = trans * this->model_matrix;

        //shader.setUniform("mycolor", window->rgba);
        shader.setUniform("p_m", window->cam->getProjMatrix());
        shader.setUniform("v_m", window->cam->getViewMatrix());
        shader.setUniform("m_m", this->model_matrix);

        glActiveTexture(GL_TEXTURE0);
        int i = 0;
        glBindTextureUnit(i, tex_ID);
        shader.setUniform("tex0", i);

        // call draw() on mesh (all meshes)
        //std::cout << "drawing model" << std::endl;
        for (auto & mesh : meshes) {
            mesh.draw(origin+offset, orientation+rotation);
        }
    }

    void clear(void) {
        for (auto& mesh : meshes) {
            mesh.clear();
        }
        if (tex_ID) {   // or all textures in vector...
            glDeleteTextures(1, &tex_ID);
            tex_ID = 0;
        }
    }
};