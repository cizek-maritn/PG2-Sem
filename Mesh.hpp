#pragma once

#include <string>
#include <iostream>
#include <vector>

#include <glm/glm.hpp> 
#include <glm/ext.hpp>

#include "assets.h"
#include "ShaderProgram.hpp"
                         
class Mesh {
public:
    // mesh data
    glm::vec3 origin{};
    glm::vec3 orientation{};
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    
    GLuint texture_id{0}; // texture id=0  means no texture
    GLenum primitive_type = GL_TRIANGLES;
    ShaderProgram shader;
    
    // mesh material
    glm::vec3 aMat{1.0f}; //white, non-transparent 
    glm::vec3 dMat{1.0f}; //white, non-transparent 
    glm::vec3 sMat{1.0f}; //white, non-transparent
    float shine{1.0f}; 
    bool MTL;
    
    // indirect (indexed) draw 
    Mesh(void) = default;
	Mesh(GLenum primitive_type, ShaderProgram shader, std::vector<Vertex> const & vertices, std::vector<GLuint> const & indices, glm::vec3 const & origin, glm::vec3 const & orientation, glm::vec3 aMat, glm::vec3 dMat, glm::vec3 sMat, float shine, bool MTL, GLuint const texture_id = 0);

    void draw(glm::vec3 const& offset, glm::vec3 const& rotation);
    void clear(void);

private:
    // OpenGL buffer IDs
    // ID = 0 is reserved (i.e. uninitalized)
     unsigned int VAO{0}, VBO{0}, EBO{0};
};
  


