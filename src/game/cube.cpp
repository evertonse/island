#pragma once
#include "cyx.hpp"
using namespace cyx;
namespace island {
    struct Block {
        // Define the cube's geometry vertices
        std::array<f32,8*3> vertices = {
            // front face
            -0.5f, -0.5f, 0.5f,   // bottom-left
            0.5f, -0.5f, 0.5f,    // bottom-right
            0.5f, 0.5f, 0.5f,     // top-right
            -0.5f, 0.5f, 0.5f,    // top-left
            // back face
            -0.5f, -0.5f, -0.5f,  // bottom-left
            0.5f, -0.5f, -0.5f,   // bottom-right
            0.5f, 0.5f, -0.5f,    // top-right
            -0.5f, 0.5f, -0.5f    // top-left
        };

        // Define the cube's geometry indices
        std::array<u32,12*3> indices = {
            // front face
            0, 1, 2,
            2, 3, 0,
            // back face
            4, 5, 6,
            6, 7, 4,
            // left face
            0, 3, 7,
            7, 4, 0,
            // right face
            1, 2, 6,
            6, 5, 1,
            // top face
            3, 2, 6,
            6, 7, 3,
            // bottom face
            0, 1, 5,
            5, 4, 0
        };

        // Create and bind the vertex buffer
        void draw() {

            unsigned int vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

            // Create and bind the index buffer
            unsigned int ibo;
            glGenBuffers(1, &ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

            // Enable and set the vertex position attribute
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            // Render the cube
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // Disable the vertex position attribute
            glDisableVertexAttribArray(0);

            // Delete the vertex and index buffers
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ibo);
        }
    };
}