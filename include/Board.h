#ifndef BOARD_H
#define BOARD_H

#include "Cube.h"
#include "Piece.h"
#include "TextRenderer.h"
#include <vector>
#include <random>
#include <glm/glm.hpp>

enum class GameState {
    WAITING_TO_START,
    PLAYING,
    GAME_OVER
};

class Board {
public:
    Board();
    ~Board();
    
    void render();
    void update();
    void startGame();
    void resetGame();
    void moveCurrentPiece(int dx, int dy);
    void rotateCurrentPiece(); 
    void rotateFallingPiece(float dx, float dy, float dz);


    void dropCurrentPiece();
    
    GameState getGameState() const { return gameState; }
    int getScore() const { return score; }
    int getLinesCleared() const { return linesCleared; }

private:
    static const int FIELD_WIDTH = 10;
    static const int FIELD_HEIGHT = 20;
    
    std::vector<std::vector<Cube*>> field;
    std::vector<Cube*> walls;
    std::vector<Cube*> floorTiles;
    
    TextRenderer* textRenderer;
    
    Piece* currentPiece;
    GameState gameState;
    int score;
    int linesCleared;
    
    glm::mat4 view;
    glm::mat4 projection;
    
    std::mt19937 rng;
    std::uniform_int_distribution<int> pieceDist;
    
    void initializeWalls();
    void initializeFloor();
    void clearField();
    void spawnNewPiece();
    bool isValidPosition(const std::vector<glm::vec2>& positions);
    void lockCurrentPiece();
    void checkAndClearLines();
    bool isLineFull(int line);
    void clearLine(int line);
    void dropLinesAbove(int clearedLine);
    void renderInstructions();
};

#endif