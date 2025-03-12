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
    
    Model(const std::filesystem::path & filename, ShaderProgram shader) {
        // load mesh (all meshes) of the model, (in the future: load material of each mesh, load textures...)
        // TODO: call LoadOBJFile, LoadMTLFile (if exist), process data, create mesh and set its properties
        //    notice: you can load multiple meshes and place them to proper positions, 
        //            multiple textures (with reusing) etc. to construct single complicated Model  
        this->origin = glm::vec3(0.0f);
        this->orientation = glm::vec3(0.0f);
        this->meshes = std::vector<Mesh>{};
        this->name = filename.string();
        this->shader = shader;

        auto filetype = filename.extension().string();
        if (filetype == ".obj") {
            auto loader = OBJLoader(filename);
            meshes.push_back(loader.getMesh(shader, this->origin, this->orientation));
        }
    }

    // update position etc. based on running time
    void update(const float delta_t) {
        // origin += glm::vec3(3,0,0) * delta_t; // s = s0 + v*dt
    }
    
    void draw(glm::vec3 const & offset = glm::vec3(0.0), glm::vec3 const & rotation = glm::vec3(0.0f)) {
        // call draw() on mesh (all meshes)
        for (auto & mesh : meshes) {
            mesh.draw(origin+offset, orientation+rotation);
        }
    }
};

