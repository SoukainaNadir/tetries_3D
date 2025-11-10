#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game() : fallTimer(0), fallInterval(1.0f), gameOver(false) {
    srand(time(0));
}

void Game::init() {
    board.init();
    spawnPiece();
}

void Game::spawnPiece() {
    PieceType type = static_cast<PieceType>(rand() % 6); 
    float randomX = rand() % (Board::WIDTH - 4);
    currentPiece = std::make_unique<Piece>(type, glm::vec3(randomX, 10.0f, 0.0f));
    currentPiece->init();
}

void Game::update(float deltaTime) {
    if(gameOver) return;
    
    fallTimer += deltaTime;
    std::cout << "Timer: " << fallTimer << std::endl; 
    
    if(fallTimer >= fallInterval) {
        std::cout << "Piece falling!" << std::endl; 
        fallTimer = 0;
        
        bool willCollide = false;
        for(auto& cube : currentPiece->cubes) {
            glm::vec3 nextPos = cube.position;
            nextPos.y -= 1.0f;
            if(!board.isPositionValid(nextPos)) {
                willCollide = true;
                break;
            }
        }
        
        if(willCollide) {
            std::cout << "Collision!" << std::endl;
            lockPiece();
            spawnPiece();
        } else {
            std::cout << "Moving down" << std::endl;
            currentPiece->moveDown();
        }
    }
}

void Game::lockPiece() {
    for(auto& cube : currentPiece->cubes) {
        if(cube.position.y >= Board::HEIGHT) {
            gameOver = true;
            std::cout << "GAME OVER!" << std::endl;
            return;
        }
        board.addCube(cube);
    }
    board.checkAndClearLines();
}

void Game::moveLeft() {
    currentPiece->moveLeft();
    for(auto& cube : currentPiece->cubes) {
        if(!board.isPositionValid(cube.position)) {
            currentPiece->moveRight();
            return;
        }
    }
}

void Game::moveRight() {
    currentPiece->moveRight();
    for(auto& cube : currentPiece->cubes) {
        if(!board.isPositionValid(cube.position)) {
            currentPiece->moveLeft();
            return;
        }
    }
}

void Game::render(GLuint shaderProgram) {
    board.render(shaderProgram);
    if(currentPiece) currentPiece->render(shaderProgram);
}