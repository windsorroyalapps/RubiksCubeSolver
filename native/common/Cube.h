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

    // SiGN / WCA-style: 2R, Rw, 3Rw2, R', M, E, S
    static std::string movesToNotation(const std::vector<Move>& moves);

    // --- Solver helpers (3x3 focused) ---

    std::pair<Color, Color> edgeColors(Face f1, Face f2) const;
    std::array<Color, 3> cornerColors(Face f1, Face f2, Face f3) const;
    bool isWhiteCrossSolved() const;
    bool isFirstLayerSolved() const;

private:
    int n_;
    std::array<std::vector<std::vector<Color>>, 6> facelets_;

    void rotateFace(int face, int turns);
    void cycleSides(int face, int depth, int turns);
    static int normalizeTurns(int turns);
    void rotateFaceCW(int face);

    void edgeCoords(Face f1, Face f2, int& faceA, int& rowA, int& colA,
                    int& faceB, int& rowB, int& colB) const;
};
