#include "Mesh.hpp"

Mesh::Mesh(GLenum primitive_type, ShaderProgram shader, std::vector<Vertex> const& vertices, std::vector<GLuint> const& indices, glm::vec3 const& origin, glm::vec3 const& orientation, GLuint const texture_id) :
    primitive_type(primitive_type),
    shader(shader),
    vertices(vertices),
    indices(indices),
    origin(origin),
    orientation(orientation),
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

void Mesh::draw(glm::vec3 const& offset, glm::vec3 const& rotation) {
    if (VAO == 0) {
        std::cerr << "VAO not initialized!\n";
        return;
    }

    shader.activate();

    // for future use: set uniform variables: position, textures, etc...  
    //set texture id etc...
    //if (texture_id > 0) {
    //    ...
    //}

    //TODO: draw mesh: bind vertex array object, draw all elements with selected primitive type 
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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