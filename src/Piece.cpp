#include "Piece.h"
#include <random>
#include <ctime>

// Girly pastel colors for pieces
static const glm::vec3 PIECE_COLORS[] = {
    glm::vec3(0.6f, 0.9f, 0.95f),   // I - Baby blue
    glm::vec3(0.9f, 0.5f, 0.8f),    // T - Pink
    glm::vec3(0.7f, 0.95f, 0.75f),  // S - Mint green
    glm::vec3(0.95f, 0.6f, 0.6f),   // Z - Coral
    glm::vec3(0.7f, 0.6f, 0.95f),   // J - Lavender
    glm::vec3(1.0f, 0.75f, 0.5f)    // L - Peach
};

Piece::Piece(PieceType type, float x, float y) : type(type), x(x), y(y) {
    color = PIECE_COLORS[static_cast<int>(type)];
    initializePiece(type);
    updateCubePositions();
}

Piece::~Piece() {
    for (Cube* cube : cubes) delete cube;
}

void Piece::initializePiece(PieceType type) {
    shape.clear();
    
    switch (type) {
        case PieceType::I: shape = {{-2, 0}, {-1, 0}, {0, 0}, {1, 0}}; break;
        case PieceType::T: shape = {{0, 0}, {-1, 0}, {1, 0}, {0, 1}}; break;
        case PieceType::S: shape = {{0, 0}, {0, 1}, {1, 1}, {1, 2}}; break;
        case PieceType::Z: shape = {{1, 0}, {1, 1}, {0, 1}, {0, 2}}; break;
        case PieceType::J: shape = {{0, 0}, {0, 1}, {0, -1}, {-1, -1}}; break;
        case PieceType::L: shape = {{0, 0}, {0, 1}, {0, -1}, {1, -1}}; break;
        default: shape = {{-2, 0}, {-1, 0}, {0, 0}, {1, 0}}; break;
    }
    
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
    for (Cube* cube : cubes) cube->render(view, projection);
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
    return PIECE_COLORS[0];
}