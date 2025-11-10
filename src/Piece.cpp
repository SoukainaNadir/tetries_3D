#include "Piece.h"
#include <cstdlib>

Piece::Piece(PieceType t, glm::vec3 pos) : type(t), position(pos) {
    color = glm::vec3((rand()%100)/100.0f, (rand()%100)/100.0f, (rand()%100)/100.0f);
    createShape();
}

void Piece::createShape() {
    cubes.clear();
    glm::vec3 offsets[4];
    
    switch(type) {
        case I: offsets[0]={0,0,0}; offsets[1]={1,0,0}; offsets[2]={2,0,0}; offsets[3]={3,0,0}; break;
        case T: offsets[0]={0,0,0}; offsets[1]={1,0,0}; offsets[2]={2,0,0}; offsets[3]={1,1,0}; break;
        case L: offsets[0]={0,0,0}; offsets[1]={0,1,0}; offsets[2]={0,2,0}; offsets[3]={1,0,0}; break;
        case J: offsets[0]={1,0,0}; offsets[1]={1,1,0}; offsets[2]={1,2,0}; offsets[3]={0,0,0}; break;
        case S:
            offsets[0] = {1,0,0};
            offsets[1] = {2,0,0};
            offsets[2] = {0,1,0};
            offsets[3] = {1,1,0};
            break;

        case Z:
            offsets[0] = {0,0,0};
            offsets[1] = {1,0,0};
            offsets[2] = {1,1,0};
            offsets[3] = {2,1,0};
            break;



    }

    for(int i=0; i<4; i++) {
        cubes.push_back(Cube(position + offsets[i], color));
    }
}

void Piece::init() {
    for(auto& cube : cubes) cube.init();
}

void Piece::render(GLuint shaderProgram) {
    for(auto& cube : cubes) cube.render(shaderProgram);
}

void Piece::moveLeft() {
    position.x -= 1.0f;
    for(auto& cube : cubes) cube.position.x -= 1.0f;
}

void Piece::moveRight() {
    position.x += 1.0f;
    for(auto& cube : cubes) cube.position.x += 1.0f;
}

void Piece::moveDown() {
    position.y -= 1.0f;
    for(auto& cube : cubes) cube.position.y -= 1.0f;
}