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

void updateWindowTitle(GameState state, int score, int lines) {
    std::string title = "Tetris 3D - ";
    switch (state) {
        case GameState::WAITING_TO_START: title += "Press SPACE to Start"; break;
        case GameState::PLAYING: title += "Score: " + std::to_string(score) + " | Lines: " + std::to_string(lines); break;
        case GameState::GAME_OVER: title += "GAME OVER | Score: " + std::to_string(score); break;
    }
    glfwSetWindowTitle(window, title.c_str());
}

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
                case GLFW_KEY_S: case GLFW_KEY_DOWN:
                    if (state == GameState::PLAYING) board->dropCurrentPiece();
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
    
    std::cout << "\n=== TETRIS 3D ===\nSPACE: Start | A/E: Move | S: Drop | ESC: Quit\n" << std::endl;
    board = new Board();

    auto lastTime = std::chrono::high_resolution_clock::now();
    float dropTimer = 0.0f;
    GameState lastState = GameState::WAITING_TO_START;

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        GameState currentState = board->getGameState();
        if (currentState != lastState) {
            updateWindowTitle(currentState, board->getScore(), board->getLinesCleared());
            lastState = currentState;
        }

        if (board->getGameState() == GameState::PLAYING) {
            dropTimer += deltaTime;
            if (dropTimer >= 0.4f) {
                board->update();
                dropTimer = 0.0f;
            }
        }

        // Soft pink background
        glClearColor(0.98f, 0.92f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        board->render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete board;
    glfwTerminate();
    return 0;
}