#pragma once

#include <array>
#include <string>
#include <vector>

// Facelet model for arbitrary n (works for 3x3 up to huge n)
enum class Color : uint8_t {
    U = 0, // White / Up
    D,     // Yellow / Down
    F,     // Green / Front
    B,     // Blue / Back
    L,     // Orange / Left
    R      // Red / Right
};

struct Move {
    int face;   // 0-5
    int depth;  // layer depth (0 = outer)
    int turns;  // 1, 2, or -1 (3)
};

class Cube {
public:
    explicit Cube(int size = 3);

    void apply(const Move& m);
    void apply(const std::vector<Move>& sequence);
    bool isSolved() const;

    int size() const { return n_; }
    std::string toString() const;

private:
    int n_;
    // facelets_[face][row][col]
    std::array<std::vector<std::vector<Color>>, 6> facelets_;

    void rotateFace(int face, int turns);
    void cycleSides(int face, int depth, int turns);
};
