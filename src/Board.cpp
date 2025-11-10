#include "Board.h"
#include <algorithm>
Board::Board() {
    floor = Cube(glm::vec3(WIDTH/2.0f - 0.5f, -1.0f, 0.0f), glm::vec3(0.3f, 0.3f, 0.3f));
    
  
    for(int y = 0; y < HEIGHT; y++) {
        walls.push_back(Cube(glm::vec3(-1.0f, y, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f)));
        walls.push_back(Cube(glm::vec3(WIDTH, y, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f)));
    }
}

void Board::init() {
    floor.init();
    for(auto& wall : walls) wall.init();
}

void Board::render(GLuint shaderProgram) {
    floor.render(shaderProgram);
    for(auto& wall : walls) wall.render(shaderProgram);
    for(auto& cube : settledCubes) cube.render(shaderProgram);
}

void Board::addCube(Cube cube) {
    settledCubes.push_back(cube);
}

bool Board::isPositionValid(glm::vec3 pos) {
    if(pos.x < 0 || pos.x >= WIDTH || pos.y < 0 || pos.y >= HEIGHT) 
        return false;
    
    for(auto& cube : settledCubes) {
        if(glm::length(cube.position - pos) < 0.1f) 
            return false;
    }
    return true;
}

void Board::checkAndClearLines() {
    for(int y = 0; y < HEIGHT; y++) {
        int count = 0;
        for(auto& cube : settledCubes) {
            if(cube.position.y == y) count++;
        }
        
        if(count >= WIDTH) {
            settledCubes.erase(
                std::remove_if(settledCubes.begin(), settledCubes.end(),
                    [y](Cube& c){ return c.position.y == y; }),
                settledCubes.end()
            );
            
            for(auto& cube : settledCubes) {
                if(cube.position.y > y) cube.position.y -= 1.0f;
            }
        }
    }
}