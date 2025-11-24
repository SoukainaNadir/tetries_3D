#ifndef CUBE_H
#define CUBE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Cube {
public:
    Cube();
    Cube(float x, float y, float z, glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f));
    ~Cube();

    void setPosition(float x, float y, float z);
    void setColor(glm::vec3 color);
    void render(const glm::mat4& view, const glm::mat4& projection);
    
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getColor() const { return color; }

private:
    void setupMesh();
    void createShaders();

    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 rotation; 
    
    unsigned int VAO, VBO, EBO;
    unsigned int edgeVAO, edgeVBO;
    unsigned int shaderProgram;
    unsigned int edgeShaderProgram;
    
    // Géométrie du cube avec les normales pour l'éclairage
    static const float vertices[];
    static const unsigned int indices[];
    
    // Code source des shaders
    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
};

#endif