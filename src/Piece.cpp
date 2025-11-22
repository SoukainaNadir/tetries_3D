#include "Piece.h"
#include <random>
#include <ctime>

Piece::Piece(PieceType type, float x, float y) : type(type), x(x), y(y) {
    color = getRandomColor();
    initializePiece(type);
    updateCubePositions();
}

Piece::~Piece() {
    for (Cube* cube : cubes) {
        delete cube;
    }
}

void Piece::initializePiece(PieceType type) {
    shape.clear();
    
    switch (type) {
        case PieceType::I: // Line piece (horizontal)
            shape = {{-2, 0}, {-1, 0}, {0, 0}, {1, 0}};
            break;
        case PieceType::T: // T piece
            shape = {{0, 0}, {-1, 0}, {1, 0}, {0, 1}};
            break;
        case PieceType::S: // S piece 
            shape = {{0, 0}, {0, 1}, {1, 1}, {1, 2}};
            break;
        case PieceType::Z: // Z piece
            shape = {{1, 0}, {1, 1}, {0, 1}, {0, 2}};
            break;
        case PieceType::J: // J piece
            shape = {{0, 0}, {0, 1}, {0, -1}, {-1, -1}};
            break;
        case PieceType::L: // L piece
            shape = {{0, 0}, {0, 1}, {0, -1}, {1, -1}};
            break;
        default: // Fallback to I piece
            shape = {{-2, 0}, {-1, 0}, {0, 0}, {1, 0}};
            break;
    }
    
    // Create cubes for each block
    for (size_t i = 0; i < shape.size(); i++) {
        cubes.push_back(new Cube(0, 0, 0, color));
    }
}

void Piece::updateCubePositions() {
    for (size_t i = 0; i < cubes.size(); i++) {
        cubes[i]->setPosition(x + shape[i].x, y + shape[i].y, 0);
        cubes[i]->setColor(color);
    }
}

void Piece::render(const glm::mat4& view, const glm::mat4& projection) {
    for (Cube* cube : cubes) {
        cube->render(view, projection);
    }
}

void Piece::move(float dx, float dy) {
    x += dx;
    y += dy;
    updateCubePositions();
}

void Piece::setPosition(float x, float y) {
    this->x = x;
    this->y = y;
    updateCubePositions();
}

std::vector<glm::vec2> Piece::getBlockPositions() const {
    std::vector<glm::vec2> positions;
    for (const auto& block : shape) {
        positions.push_back({x + block.x, y + block.y});
    }
    return positions;
}

glm::vec3 Piece::getRandomColor() {
    static std::mt19937 rng(time(0));
    static std::uniform_real_distribution<float> dist(0.2f, 0.9f);
    
    // Generate bright, saturated colors
    float r = dist(rng);
    float g = dist(rng);
    float b = dist(rng);
    
    // Ensure at least one component is bright
    float maxComp = std::max({r, g, b});
    if (maxComp < 0.7f) {
        if (r == maxComp) r = 0.8f;
        else if (g == maxComp) g = 0.8f;
        else b = 0.8f;
    }
    
    return glm::vec3(r, g, b);
}