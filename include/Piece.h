#ifndef PIECE_H
#define PIECE_H

#include "Cube.h"
#include <vector>
#include <glm/glm.hpp>

enum class PieceType {
    I = 0, T = 1, S = 2, Z = 3, J = 4, L = 5
};

class Piece {
public:
    Piece(PieceType type, float x, float y);
    ~Piece();
    
    void render(const glm::mat4& view, const glm::mat4& projection);
    void move(float dx, float dy);
    void setPosition(float x, float y);
    
    std::vector<glm::vec2> getBlockPositions() const;
    glm::vec3 getColor() const { return color; }
    
private:
    void initializePiece(PieceType type);
    void updateCubePositions();
    glm::vec3 getRandomColor();
    
    PieceType type;
    float x, y;
    std::vector<glm::vec2> shape;
    std::vector<Cube*> cubes;
    glm::vec3 color;
};

#endif