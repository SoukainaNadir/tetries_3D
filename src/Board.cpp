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
    
    textRenderer = new TextRenderer();
    textRenderer->setProjection(1200, 900);
    
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
    delete textRenderer;
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
    glm::vec3 frameColor(0.6f, 0.6f, 0.65f);
    
    for (int x = -1; x <= FIELD_WIDTH; x++) {
        walls.push_back(new Cube(x, -1, 0, frameColor));
    }
    
    for (int y = 0; y <= FIELD_HEIGHT; y++) {
        walls.push_back(new Cube(-1, y, 0, frameColor));
    }
    
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

void Board::rotateCurrentPiece() {
    if (!currentPiece || gameState != GameState::PLAYING) return;
    
    std::vector<glm::vec2> oldPositions = currentPiece->getBlockPositions();
    
    currentPiece->rotate();
    
    if (!isValidPosition(currentPiece->getBlockPositions())) {
        currentPiece->move(1, 0);
        if (!isValidPosition(currentPiece->getBlockPositions())) {
            currentPiece->move(-2, 0);
            if (!isValidPosition(currentPiece->getBlockPositions())) {
                currentPiece->move(1, 1);
                if (!isValidPosition(currentPiece->getBlockPositions())) {
                    currentPiece->move(0, -1);
                    currentPiece->rotate();
                    currentPiece->rotate();
                    currentPiece->rotate();
                }
            }
        }
    }
}

void Board::dropCurrentPiece() {
    if (!currentPiece || gameState != GameState::PLAYING) return;
    
    currentPiece->move(0, -1);
    if (!isValidPosition(currentPiece->getBlockPositions())) {
        currentPiece->move(0, 1);
        lockCurrentPiece();
    }
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

void Board::renderInstructions() {
    glDisable(GL_DEPTH_TEST);
    
    glm::vec3 titleColor(0.85f, 0.45f, 0.55f);
    glm::vec3 subtitleColor(0.55f, 0.45f, 0.65f);
    glm::vec3 keyColor(0.45f, 0.65f, 0.75f);
    glm::vec3 textColor(0.5f, 0.5f, 0.55f);
    glm::vec3 scoreColor(0.5f, 0.75f, 0.6f);
    glm::vec3 accentColor(0.95f, 0.7f, 0.5f);
    glm::vec3 dimColor(0.65f, 0.65f, 0.7f);
    
    textRenderer->renderText("TETRIS 3D", 460, 850, 5.0f, titleColor);
    
    float leftX = 50.0f;
    
    if (gameState == GameState::WAITING_TO_START) {
        textRenderer->renderText("PRESS", leftX, 500, 3.5f, titleColor);
        textRenderer->renderText("SPACE", leftX, 460, 3.5f, titleColor);
        textRenderer->renderText("TO", leftX + 30, 420, 3.0f, dimColor);
        textRenderer->renderText("START!", leftX, 380, 3.5f, titleColor);
    } else if (gameState == GameState::GAME_OVER) {
        textRenderer->renderText("GAME", leftX, 500, 4.0f, glm::vec3(0.9f, 0.3f, 0.4f));
        textRenderer->renderText("OVER", leftX, 450, 4.0f, glm::vec3(0.9f, 0.3f, 0.4f));
        textRenderer->renderText("------", leftX, 420, 2.5f, glm::vec3(0.9f, 0.3f, 0.4f));
        textRenderer->renderText("SPACE", leftX, 370, 2.8f, keyColor);
        textRenderer->renderText("to", leftX + 25, 330, 2.5f, dimColor);
        textRenderer->renderText("retry", leftX + 10, 290, 2.8f, textColor);
    } else if (gameState == GameState::PLAYING) {
        textRenderer->renderText("STATUS", leftX, 500, 2.5f, dimColor);
        textRenderer->renderText("PLAYING", leftX, 460, 3.0f, scoreColor);
    }
    
    float rightX = 850.0f;
    
    textRenderer->renderText("SCORE", rightX, 780, 3.5f, subtitleColor);
    textRenderer->renderText("--------", rightX, 750, 2.5f, dimColor);
        
    // Score avec des zéros devant 
    std::string scoreStr = std::to_string(score);
    while (scoreStr.length() < 6) scoreStr = "0" + scoreStr;
    textRenderer->renderText(scoreStr, rightX, 700, 4.0f, scoreColor);
    
    textRenderer->renderText("LINES", rightX, 620, 3.5f, subtitleColor);
    textRenderer->renderText(std::to_string(linesCleared), rightX, 570, 4.0f, accentColor);
    
    textRenderer->renderText("CONTROLS", rightX, 490, 3.0f, subtitleColor);
    textRenderer->renderText("----------", rightX, 465, 2.5f, dimColor);
    
    float ctrlY = 420.0f;
    float spacing = 60.0f;
    float scale = 2.5f;
    
    textRenderer->renderText("[A]", rightX, ctrlY, scale, keyColor);
    textRenderer->renderText("or", rightX + 70, ctrlY, 2.0f, dimColor);
    textRenderer->renderText("[<-]", rightX + 105, ctrlY, scale, keyColor);
    textRenderer->renderText("Left", rightX, ctrlY - 30, scale, textColor);
    
    textRenderer->renderText("[E]", rightX, ctrlY - spacing, scale, keyColor);
    textRenderer->renderText("or", rightX + 70, ctrlY - spacing, 2.0f, dimColor);
    textRenderer->renderText("[->]", rightX + 105, ctrlY - spacing, scale, keyColor);
    textRenderer->renderText("Right", rightX, ctrlY - spacing - 30, scale, textColor);
    
    textRenderer->renderText("[S]", rightX, ctrlY - spacing * 2, scale, keyColor);
    textRenderer->renderText("or", rightX + 70, ctrlY - spacing * 2, 2.0f, dimColor);
    textRenderer->renderText("[v]", rightX + 105, ctrlY - spacing * 2, scale, keyColor);
    textRenderer->renderText("Down", rightX, ctrlY - spacing * 2 - 30, scale, textColor);
    
    textRenderer->renderText("[UP]", rightX, ctrlY - spacing * 3, scale, keyColor);
    textRenderer->renderText("or", rightX + 80, ctrlY - spacing * 3, 2.0f, dimColor);
    textRenderer->renderText("[W]", rightX + 115, ctrlY - spacing * 3, scale, keyColor);
    textRenderer->renderText("Rotate", rightX, ctrlY - spacing * 3 - 30, scale, textColor);
    
    textRenderer->renderText("[SPACE]", rightX, ctrlY - spacing * 4, scale, keyColor);
    textRenderer->renderText("Play", rightX, ctrlY - spacing * 4 - 30, scale, textColor);
    
    textRenderer->renderText("[ESC]", rightX, ctrlY - spacing * 5, scale, keyColor);
    textRenderer->renderText("Quit", rightX, ctrlY - spacing * 5 - 30, scale, textColor);
    
    glEnable(GL_DEPTH_TEST);
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
    
    renderInstructions();
}