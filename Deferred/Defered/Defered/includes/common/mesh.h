#pragma once

#include <glad/glad.h>

#include <assimp/Importer.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

using namespace std;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

struct Texture {
    unsigned int id;
    string type;
    aiString path;
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    unsigned int vao;

    Mesh(const vector<Vertex> vertices, const vector<unsigned int> indices)
		: vertices(vertices)
		, indices(indices)
    {
        setup_mesh();
    }

    void draw() const
    {        
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

private:
    unsigned int vbo_, ebo_;

    void setup_mesh()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo_);
        glGenBuffers(1, &ebo_);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(0));

        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

        glBindVertexArray(0);
    }
};