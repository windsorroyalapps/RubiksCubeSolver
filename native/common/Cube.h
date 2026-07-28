#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

enum Face : int {
    U = 0, D = 1, F = 2, B = 3, L = 4, R = 5
};

enum class Color : uint8_t {
    U = 0, D, F, B, L, R
};

struct Move {
    int face;
    int depth;
    int turns;
};

class Cube {
public:
    explicit Cube(int size = 3);

    void apply(const Move& m);
    void apply(const std::vector<Move>& sequence);
    bool isSolved() const;

    int size() const { return n_; }

    Color get(int face, int row, int col) const;
    void set(int face, int row, int col, Color c);

    std::string toString() const;
    std::string faceToString(int face) const;
    void applyNotation(const std::string& notation);

    // --- Solver helpers (3x3 focused) ---

    // Edge positions: 12 edges. Returns facelet colors of an edge at (face, slot)
    // slot: 0=U, 1=R, 2=D, 3=L relative to the face for side faces, etc.
    // Simplified: get the two colors of the edge currently at a named position.
    // Positions encoded as pair of faces that the edge sits between.
    std::pair<Color, Color> edgeColors(Face f1, Face f2) const;

    // Corner colors at intersection of three faces
    std::array<Color, 3> cornerColors(Face f1, Face f2, Face f3) const;

    // Check if white cross is solved (white edges on D, correctly aligned)
    bool isWhiteCrossSolved() const;

    // Check if first layer (white face + sides) is solved
    bool isFirstLayerSolved() const;

private:
    int n_;
    std::array<std::vector<std::vector<Color>>, 6> facelets_;

    void rotateFace(int face, int turns);
    void cycleSides(int face, int depth, int turns);
    static int normalizeTurns(int turns);
    void rotateFaceCW(int face);

    // Map edge between two faces to facelet coordinates
    void edgeCoords(Face f1, Face f2, int& faceA, int& rowA, int& colA,
                    int& faceB, int& rowB, int& colB) const;
};
