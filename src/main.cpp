#include "Board.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
                case GLFW_KEY_UP: case GLFW_KEY_W:  
                    if (state == GameState::PLAYING) board->rotateCurrentPiece();
                    break;
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, true);
                    break;
            }
        }
    }
}

void setWindowIcon(GLFWwindow* window, const char* iconPath) {
    int width, height, channels;
    
    unsigned char* pixels = stbi_load(iconPath, &width, &height, &channels, 4);
    
    if (pixels) {
        GLFWimage icon;
        icon.width = width;
        icon.height = height;
        icon.pixels = pixels;
        
        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(pixels);
        std::cout << "Icône chargée avec succès!" << std::endl;
    } else {
        std::cout << "Impossible de charger l'icône depuis: " << iconPath << std::endl;
        std::cout << "Vérifiez que stb_image.h est téléchargé et que le chemin icon.png est correct." << std::endl;
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tetris 3D", NULL, NULL);
    if (!window) {
        std::cout << "Erreur lors de la création de la fenêtre" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // Chargement de l'icône personnalisée
    setWindowIcon(window, "../icon.png");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Échec de l'initialisation de GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    
    board = new Board();

    auto lastTime = std::chrono::high_resolution_clock::now();
    float dropTimer = 0.0f;
    
    const float normalSpeed = 0.4f;
    const float fastSpeed = 0.05f;

    // La boucle principale du jeu 
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        if (board->getGameState() == GameState::PLAYING) {
            dropTimer += deltaTime;
            
            // Vitesse rapide si on appuie sur S ou flèche du bas
            bool fastDrop = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || 
                           glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
            
            float currentSpeed = fastDrop ? fastSpeed : normalSpeed;
            
            if (dropTimer >= currentSpeed) {
                board->update();
                dropTimer = 0.0f;
            }
        }

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