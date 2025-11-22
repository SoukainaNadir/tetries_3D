#include "Board.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <ctime>

Board::Board() : gameState(GameState::WAITING_TO_START), currentPiece(nullptr), 
                 score(0), linesCleared(0), rng(std::time(0)), pieceDist(0, 5) {
    field.resize(FIELD_HEIGHT);
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        field[y].resize(FIELD_WIDTH, nullptr);
    }
    
    initializeWalls();
    
    view = glm::lookAt(
        glm::vec3(4.5f, 10.0f, 30.0f),
        glm::vec3(4.5f, 10.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    projection = glm::perspective(
        glm::radians(45.0f),
        1200.0f / 900.0f, 
        0.1f, 
        100.0f
    );
}

Board::~Board() {
    clearField();
    for (Cube* wall : walls) delete wall;
    for (Cube* tile : floorTiles) delete tile;
    delete currentPiece;
}

void Board::clearField() {
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            delete field[y][x];
            field[y][x] = nullptr;
        }
    }
}

void Board::initializeWalls() {
    // Gray like reference image (lighting won't affect it much)
    glm::vec3 frameColor(0.6f, 0.6f, 0.65f);
    
    // Bottom wall
    for (int x = -1; x <= FIELD_WIDTH; x++) {
        walls.push_back(new Cube(x, -1, 0, frameColor));
    }
    
    // Left wall
    for (int y = 0; y <= FIELD_HEIGHT; y++) {
        walls.push_back(new Cube(-1, y, 0, frameColor));
    }
    
    // Right wall
    for (int y = 0; y <= FIELD_HEIGHT; y++) {
        walls.push_back(new Cube(FIELD_WIDTH, y, 0, frameColor));
    }
}

void Board::initializeFloor() {}

void Board::startGame() {
    if (gameState != GameState::WAITING_TO_START) return;
    resetGame();
    gameState = GameState::PLAYING;
    spawnNewPiece();
}

void Board::resetGame() {
    clearField();
    delete currentPiece;
    currentPiece = nullptr;
    score = 0;
    linesCleared = 0;
    gameState = GameState::WAITING_TO_START;
}

void Board::spawnNewPiece() {
    if (gameState != GameState::PLAYING) return;
    PieceType type = static_cast<PieceType>(pieceDist(rng));
    currentPiece = new Piece(type, 5.0f, FIELD_HEIGHT);
    
    if (!isValidPosition(currentPiece->getBlockPositions())) {
        gameState = GameState::GAME_OVER;
        delete currentPiece;
        currentPiece = nullptr;
    }
}

bool Board::isValidPosition(const std::vector<glm::vec2>& positions) {
    for (const glm::vec2& pos : positions) {
        int x = static_cast<int>(pos.x);
        int y = static_cast<int>(pos.y);
        if (x < 0 || x >= FIELD_WIDTH || y < 0) return false;
        if (y < FIELD_HEIGHT && field[y][x] != nullptr) return false;
    }
    return true;
}

void Board::moveCurrentPiece(int dx, int dy) {
    if (!currentPiece || gameState != GameState::PLAYING) return;
    currentPiece->move(dx, dy);
    if (!isValidPosition(currentPiece->getBlockPositions())) {
        currentPiece->move(-dx, -dy);
    }
}

void Board::dropCurrentPiece() {
    if (!currentPiece || gameState != GameState::PLAYING) return;
    while (true) {
        currentPiece->move(0, -1);
        if (!isValidPosition(currentPiece->getBlockPositions())) {
            currentPiece->move(0, 1);
            break;
        }
    }
    lockCurrentPiece();
}

void Board::update() {
    if (!currentPiece || gameState != GameState::PLAYING) return;
    currentPiece->move(0, -1);
    if (!isValidPosition(currentPiece->getBlockPositions())) {
        currentPiece->move(0, 1);
        lockCurrentPiece();
    }
}

void Board::lockCurrentPiece() {
    if (!currentPiece) return;
    
    std::vector<glm::vec2> positions = currentPiece->getBlockPositions();
    glm::vec3 color = currentPiece->getColor();
    
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

void Board::checkAndClearLines() {
    int clearedThisTurn = 0;
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        if (isLineFull(y)) {
            clearLine(y);
            dropLinesAbove(y);
            clearedThisTurn++;
            y--;
        }
    }
    if (clearedThisTurn > 0) {
        linesCleared += clearedThisTurn;
        score += clearedThisTurn * 100 * (clearedThisTurn > 1 ? 2 : 1);
    }
}

bool Board::isLineFull(int line) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
        if (field[line][x] == nullptr) return false;
    }
    return true;
}

void Board::clearLine(int line) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
        delete field[line][x];
        field[line][x] = nullptr;
    }
}

void Board::dropLinesAbove(int clearedLine) {
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

void Board::render() {
    for (Cube* tile : floorTiles) tile->render(view, projection);
    for (Cube* wall : walls) wall->render(view, projection);
    
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (field[y][x] != nullptr) field[y][x]->render(view, projection);
        }
    }
    
    if (currentPiece && gameState == GameState::PLAYING) {
        currentPiece->render(view, projection);
    }
}