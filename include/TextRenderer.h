#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
    void setProjection(int screenWidth, int screenHeight);

private:
    unsigned int VAO, VBO;
    unsigned int shaderProgram;
    glm::mat4 projection;
    
    void initShaders();
    void initBuffers();
};

#endif