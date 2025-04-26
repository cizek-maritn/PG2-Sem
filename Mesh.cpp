#include "Mesh.hpp"

Mesh::Mesh(GLenum primitive_type, ShaderProgram shader, std::vector<Vertex> const& vertices, std::vector<GLuint> const& indices, glm::vec3 const& origin, glm::vec3 const& orientation, glm::vec3 aMat, glm::vec3 dMat, glm::vec3 sMat, float shine, bool MTL, GLuint const texture_id) :
    primitive_type(primitive_type),
    shader(shader),
    vertices(vertices),
    indices(indices),
    origin(origin),
    orientation(orientation),
    aMat(aMat),
    dMat(dMat),
    sMat(sMat),
    shine(shine),
    MTL(MTL),
    texture_id(texture_id)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0 + offsetof(Vertex, Position)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0 + offsetof(Vertex, Normal)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0 + offsetof(Vertex, TexCoords)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

void Mesh::draw(glm::vec3 const& offset, glm::vec3 const& rotation) {
    if (VAO == 0) {
        std::cerr << "VAO not initialized!\n";
        return;
    }

    //std::cout << "Drawing mesh with VAO: " << VAO << ", Index count: " << indices.size() << std::endl;

    shader.activate();
    if (MTL) {
        shader.setUniform("useTexture", 0);
        shader.setUniform("ambient_material", aMat);
        shader.setUniform("diffuse_material", dMat);
        shader.setUniform("specular_material", sMat);
        shader.setUniform("specular_shinines", shine);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //set back to "default"
    if (MTL) {
        shader.setUniform("useTexture", 1);
        shader.setUniform("ambient_material", glm::vec3(1.0f));
        shader.setUniform("diffuse_material", glm::vec3(0.8f));
        shader.setUniform("specular_material", glm::vec3(1.0f));
        shader.setUniform("specular_shinines", 512.0f);
    }

}

void Mesh::clear(void) {
    texture_id = 0;
    primitive_type = GL_POINT;
    // TODO: clear rest of the member variables to safe default
    vertices.clear();
    indices.clear();
    // TODO: delete all allocations 
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
};