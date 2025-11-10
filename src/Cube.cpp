#include "Cube.h"

static float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f
};

static unsigned int indices[] = {
    0,1,2, 2,3,0,  4,5,6, 6,7,4,  0,4,7, 7,3,0,
    1,5,6, 6,2,1,  3,2,6, 6,7,3,  0,1,5, 5,4,0
};

Cube::Cube() : position(0.0f), color(1.0f), initialized(false) {}

Cube::Cube(glm::vec3 pos, glm::vec3 col) : position(pos), color(col), initialized(false) {}

Cube::~Cube() { cleanup(); }

void Cube::init() {
    if (initialized) return;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    initialized = true;
}

void Cube::render(GLuint shaderProgram) {
    if (!initialized) return;
    
    glUseProgram(shaderProgram);
    glUniform3fv(glGetUniformLocation(shaderProgram, "cubePosition"), 1, &position[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "cubeColor"), 1, &color[0]);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Cube::cleanup() {
    if (initialized) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        initialized = false;
    }
}