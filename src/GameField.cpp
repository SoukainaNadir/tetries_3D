#include "GameField.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

GameField::GameField() : gameState(GameState::WAITING_TO_START), currentPiece(nullptr), 
                         score(0), linesCleared(0), rng(std::time(0)), pieceDist(0, 5) {
    // Initialize field grid
    field.resize(FIELD_HEIGHT);
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        field[y].resize(FIELD_WIDTH, nullptr);
    }
    
    initializeWalls();
    
    // Setup camera and projection
    view = glm::lookAt(
        glm::vec3(20.0f, 25.0f, 70.0f),
        glm::vec3(4.5f, 7.5f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    projection = glm::perspective(
        glm::radians(15.0f),
        1200.0f / 900.0f, 
        0.1f, 
        100.0f
    );
    
    // Print welcome message
    std::cout << "\n=== TETRIS 3D - WAITING TO START ===" << std::endl;
    std::cout << "Press SPACE to begin!" << std::endl;
}

GameField::~GameField() {
    clearField();
    
    // Clean up walls
    for (Cube* wall : walls) {
        delete wall;
    }
    
    delete currentPiece;
}

void GameField::clearField() {
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            delete field[y][x];
            field[y][x] = nullptr;
        }
    }
}

void GameField::initializeWalls() {
    glm::vec3 wallColor(0.3f, 0.3f, 0.3f);
    
    // Bottom wall
    for (int x = -1; x <= FIELD_WIDTH; x++) {
        walls.push_back(new Cube(x, -1, 0, wallColor));
    }
    
    // Left wall
    for (int y = 0; y < FIELD_HEIGHT + 2; y++) {
        walls.push_back(new Cube(-1, y, 0, wallColor));
    }
    
    // Right wall
    for (int y = 0; y < FIELD_HEIGHT + 2; y++) {
        walls.push_back(new Cube(FIELD_WIDTH, y, 0, wallColor));
    }
}

void GameField::startGame() {
    if (gameState != GameState::WAITING_TO_START) return;
    
    resetGame();
    gameState = GameState::PLAYING;
    spawnNewPiece();
    
    std::cout << "\n=== GAME STARTED ===" << std::endl;
    std::cout << "Score: " << score << " | Lines: " << linesCleared << std::endl;
}

void GameField::resetGame() {
    clearField();
    delete currentPiece;
    currentPiece = nullptr;
    score = 0;
    linesCleared = 0;
    gameState = GameState::WAITING_TO_START;
    
    std::cout << "\n=== GAME RESET - WAITING TO START ===" << std::endl;
    std::cout << "Press SPACE to begin!" << std::endl;
}

void GameField::spawnNewPiece() {
    if (gameState != GameState::PLAYING) return;
    
    PieceType type = static_cast<PieceType>(pieceDist(rng));
    currentPiece = new Piece(type, 5.0f, FIELD_HEIGHT);
    
    // Check if spawn position is valid
    if (!isValidPosition(currentPiece->getBlockPositions())) {
        gameState = GameState::GAME_OVER;
        std::cout << "\n=== GAME OVER ===" << std::endl;
        std::cout << "Final Score: " << score << std::endl;
        std::cout << "Lines Cleared: " << linesCleared << std::endl;
        std::cout << "Press SPACE to restart!" << std::endl;
        
        delete currentPiece;
        currentPiece = nullptr;
    }
}

bool GameField::isValidPosition(const std::vector<glm::vec2>& positions) {
    for (const glm::vec2& pos : positions) {
        int x = static_cast<int>(pos.x);
        int y = static_cast<int>(pos.y);
        
        // Check bounds
        if (x < 0 || x >= FIELD_WIDTH || y < 0) {
            return false;
        }
        
        // Check collision with placed pieces (only check if within field height)
        if (y < FIELD_HEIGHT && field[y][x] != nullptr) {
            return false;
        }
    }
    return true;
}

void GameField::moveCurrentPiece(int dx, int dy) {
    if (!currentPiece || gameState != GameState::PLAYING) return;
    
    currentPiece->move(dx, dy);
    if (!isValidPosition(currentPiece->getBlockPositions())) {
        currentPiece->move(-dx, -dy); // Revert move
    }
}

void GameField::dropCurrentPiece() {
    if (!currentPiece || gameState != GameState::PLAYING) return;
    
    while (true) {
        currentPiece->move(0, -1);
        if (!isValidPosition(currentPiece->getBlockPositions())) {
            currentPiece->move(0, 1); // Revert last move
            break;
        }
    }
    lockCurrentPiece();
}

void GameField::update() {
    if (!currentPiece || gameState != GameState::PLAYING) return;
    
    // Try to move piece down
    currentPiece->move(0, -1);
    if (!isValidPosition(currentPiece->getBlockPositions())) {
        currentPiece->move(0, 1); // Revert move
        lockCurrentPiece();
    }
}

void GameField::lockCurrentPiece() {
    if (!currentPiece) return;
    
    std::vector<glm::vec2> positions = currentPiece->getBlockPositions();
    glm::vec3 color = currentPiece->getColor();
    
    // Place cubes in field
    for (const glm::vec2& pos : positions) {
        int x = static_cast<int>(pos.x);
        int y = static_cast<int>(pos.y);
        
        if (x >= 0 && x < FIELD_WIDTH && y >= 0 && y < FIELD_HEIGHT) {
            field[y][x] = new Cube(x, y, 0, color);
        }
    }
    
    delete currentPiece;
    currentPiece = nullptr;
    
    checkAndClearLines();
    spawnNewPiece();
}

void GameField::checkAndClearLines() {
    int clearedThisTurn = 0;
    
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        if (isLineFull(y)) {
            clearLine(y);
            dropLinesAbove(y);
            clearedThisTurn++;
            y--; // Check same line again since lines dropped down
        }
    }
    
    if (clearedThisTurn > 0) {
        linesCleared += clearedThisTurn;
        score += clearedThisTurn * 100 * (clearedThisTurn > 1 ? 2 : 1); // Bonus for multiple lines
        
        std::cout << "Lines cleared: " << clearedThisTurn << " | Total: " << linesCleared << " | Score: " << score << std::endl;
    }
}

bool GameField::isLineFull(int line) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
        if (field[line][x] == nullptr) {
            return false;
        }
    }
    return true;
}

void GameField::clearLine(int line) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
        delete field[line][x];
        field[line][x] = nullptr;
    }
}

void GameField::dropLinesAbove(int clearedLine) {
    for (int y = clearedLine + 1; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (field[y][x] != nullptr) {
                field[y-1][x] = field[y][x];
                field[y-1][x]->setPosition(x, y-1, 0);
                field[y][x] = nullptr;
            }
        }
    }
}

void GameField::render() {
    // Always render walls
    for (Cube* wall : walls) {
        wall->render(view, projection);
    }
    
    // Render placed pieces
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (field[y][x] != nullptr) {
                field[y][x]->render(view, projection);
            }
        }
    }
    
    // Render current falling piece (only when playing)
    if (currentPiece && gameState == GameState::PLAYING) {
        currentPiece->render(view, projection);
    }
}