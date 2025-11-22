#include "Cube.h"
#include <iostream>

const float Cube::vertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 0
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 1
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 2
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 3

    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 4
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 5
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 6
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 7

    // Left face
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // 8
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // 9
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // 10
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // 11

    // Right face
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // 12
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // 13
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // 14
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // 15

    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // 16
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // 17
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // 18
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // 19

    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // 20
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // 21
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // 22
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f  // 23
};

const unsigned int Cube::indices[] = {
    // Front face
    0, 1, 2, 2, 3, 0,
    // Back face
    4, 5, 6, 6, 7, 4,
    // Left face
    8, 9, 10, 10, 11, 8,
    // Right face
    12, 13, 14, 14, 15, 12,
    // Bottom face
    16, 17, 18, 18, 19, 16,
    // Top face
    20, 21, 22, 22, 23, 20
};

const char* Cube::vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* Cube::fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 cubeColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
    // Ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.1;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * cubeColor;
    FragColor = vec4(result, 1.0);
}
)";

Cube::Cube() : position(0.0f), color(0.5f, 0.5f, 0.5f) {
    setupMesh();
    createShaders();
}

Cube::Cube(float x, float y, float z, glm::vec3 color) : position(x, y, z), color(color) {
    setupMesh();
    createShaders();
}

Cube::~Cube() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &edgeVAO);
    glDeleteBuffers(1, &edgeVBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(edgeShaderProgram);
}

void Cube::setupMesh() {
    // Setup face mesh
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Setup edge mesh (12 edges)
    float edgeVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, // bottom back
        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, // top back
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, // bottom front
        -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, // top front
        
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, // bottom left
         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f, // bottom right
        -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, // top left
         0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f, // top right
        
        -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, // back left
         0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f, // back right
        -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, // front left
         0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f  // front right
    };

    glGenVertexArrays(1, &edgeVAO);
    glGenBuffers(1, &edgeVBO);

    glBindVertexArray(edgeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, edgeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(edgeVertices), edgeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Cube::createShaders() {
    // Face shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Edge shaders
    const char* edgeVertexSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

    const char* edgeFragmentSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 edgeColor;

void main() {
    FragColor = vec4(edgeColor, 1.0);
}
)";

    unsigned int edgeVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(edgeVertexShader, 1, &edgeVertexSource, NULL);
    glCompileShader(edgeVertexShader);

    unsigned int edgeFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(edgeFragmentShader, 1, &edgeFragmentSource, NULL);
    glCompileShader(edgeFragmentShader);

    edgeShaderProgram = glCreateProgram();
    glAttachShader(edgeShaderProgram, edgeVertexShader);
    glAttachShader(edgeShaderProgram, edgeFragmentShader);
    glLinkProgram(edgeShaderProgram);

    glDeleteShader(edgeVertexShader);
    glDeleteShader(edgeFragmentShader);
}

void Cube::setPosition(float x, float y, float z) {
    position = glm::vec3(x, y, z);
}

void Cube::setColor(glm::vec3 color) {
    this->color = color;
}

void Cube::render(const glm::mat4& view, const glm::mat4& projection) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    // Render faces
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    glUniform3fv(glGetUniformLocation(shaderProgram, "cubeColor"), 1, glm::value_ptr(color));
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 10.0f, 15.0f, 10.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), 10.0f, 15.0f, 35.0f);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Render edges
    glUseProgram(edgeShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(edgeShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(edgeShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(edgeShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    glm::vec3 edgeColor = color * 0.3f;
    glUniform3fv(glGetUniformLocation(edgeShaderProgram, "edgeColor"), 1, glm::value_ptr(edgeColor));

    glLineWidth(2.0f);
    glBindVertexArray(edgeVAO);
    glDrawArrays(GL_LINES, 0, 24);
    
    glBindVertexArray(0);
}