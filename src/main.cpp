#include "Board.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <string>

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

Board* board = nullptr;
GLFWwindow* window = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (board) {
            GameState state = board->getGameState();
            switch (key) {
                case GLFW_KEY_SPACE:
                    if (state == GameState::WAITING_TO_START) board->startGame();
                    else if (state == GameState::GAME_OVER) board->resetGame();
                    break;
                case GLFW_KEY_A: case GLFW_KEY_LEFT:
                    if (state == GameState::PLAYING) board->moveCurrentPiece(-1, 0);
                    break;
                case GLFW_KEY_E: case GLFW_KEY_RIGHT:
                    if (state == GameState::PLAYING) board->moveCurrentPiece(1, 0);
                    break;
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, true);
                    break;
            }
        }
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tetris 3D", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    
    board = new Board();

    auto lastTime = std::chrono::high_resolution_clock::now();
    float dropTimer = 0.0f;
    
    const float normalSpeed = 0.4f;  // Normal fall speed
    const float fastSpeed = 0.05f;   // Fast fall speed when S/Down is held

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        if (board->getGameState() == GameState::PLAYING) {
            dropTimer += deltaTime;
            
            // Check if S or Down arrow is held for fast drop
            bool fastDrop = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || 
                           glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
            
            float currentSpeed = fastDrop ? fastSpeed : normalSpeed;
            
            if (dropTimer >= currentSpeed) {
                board->update();
                dropTimer = 0.0f;
            }
        }

        // Soft pink background
        glClearColor(0.96f, 0.91f, 0.94f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        board->render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete board;
    glfwTerminate();
    return 0;
} 