#include "Cube.h"
#include <stdexcept>

Cube::Cube(int size) : n_(size) {
    if (n_ < 2) throw std::invalid_argument("Cube size must be >= 2");

    for (int f = 0; f < 6; ++f) {
        facelets_[f].assign(n_, std::vector<Color>(n_, static_cast<Color>(f)));
    }
}

void Cube::apply(const Move& m) {
    // TODO: implement face rotation + side cycling for arbitrary n
    // This is the core move engine for both CFOP and Reduction
}

void Cube::apply(const std::vector<Move>& sequence) {
    for (const auto& m : sequence) {
        apply(m);
    }
}

bool Cube::isSolved() const {
    for (int f = 0; f < 6; ++f) {
        Color c = facelets_[f][0][0];
        for (int r = 0; r < n_; ++r) {
            for (int col = 0; col < n_; ++col) {
                if (facelets_[f][r][col] != c) return false;
            }
        }
    }
    return true;
}

std::string Cube::toString() const {
    // Simple debug dump
    return "Cube(" + std::to_string(n_) + "x" + std::to_string(n_) + ")";
}

void Cube::rotateFace(int /*face*/, int /*turns*/) {
    // TODO
}

void Cube::cycleSides(int /*face*/, int /*depth*/, int /*turns*/) {
    // TODO
}
