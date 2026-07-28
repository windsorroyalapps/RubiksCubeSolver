#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

// Face indices
enum Face : int {
    U = 0, // Up
    D = 1, // Down
    F = 2, // Front
    B = 3, // Back
    L = 4, // Left
    R = 5  // Right
};

enum class Color : uint8_t {
    U = 0,
    D,
    F,
    B,
    L,
    R
};

struct Move {
    int face;   // 0-5 (Face enum)
    int depth;  // 0 = outer face, 1 = next inner layer, ...
    int turns;  // 1 = 90° CW, 2 = 180°, -1 = 90° CCW
};

class Cube {
public:
    explicit Cube(int size = 3);

    void apply(const Move& m);
    void apply(const std::vector<Move>& sequence);
    bool isSolved() const;

    int size() const { return n_; }

    // Access a single facelet (row/col 0..n-1)
    Color get(int face, int row, int col) const;
    void set(int face, int row, int col, Color c);

    // Debug / serialization
    std::string toString() const;
    std::string faceToString(int face) const;

    // Parse simple Singmaster-style moves for 3x3 ("R U R' U'") and apply
    // For larger n, use explicit Move objects.
    void applyNotation(const std::string& notation);

private:
    int n_;
    // facelets_[face][row][col]
    std::array<std::vector<std::vector<Color>>, 6> facelets_;

    void rotateFace(int face, int turns);
    void cycleSides(int face, int depth, int turns);

    // Helpers
    static int normalizeTurns(int turns);
    void rotateFaceCW(int face);
};
