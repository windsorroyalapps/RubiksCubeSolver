#include "ParityHandler.h"

#include <sstream>

static std::vector<Move> parse(const std::string& notation) {
    std::vector<Move> result;
    std::istringstream iss(notation);
    std::string token;
    while (iss >> token) {
        if (token.empty()) continue;
        // Support inner-slice notation like 2R (depth=1, face=R)
        int depth = 0;
        size_t pos = 0;
        if (token[0] >= '2' && token[0] <= '9') {
            depth = token[0] - '1'; // 2R -> depth 1
            pos = 1;
        }
        if (pos >= token.size()) continue;
        int face = 0;
        switch (token[pos]) {
            case 'U': face = U; break;
            case 'D': face = D; break;
            case 'F': face = F; break;
            case 'B': face = B; break;
            case 'L': face = L; break;
            case 'R': face = R; break;
            default: continue;
        }
        int turns = 1;
        if (token.size() > pos + 1) {
            if (token[pos + 1] == '\'') turns = -1;
            else if (token[pos + 1] == '2') turns = 2;
        }
        result.push_back(Move{face, depth, turns});
    }
    return result;
}

bool ParityHandler::hasOLLParity(const Cube& c) {
    if (c.size() < 4 || c.size() % 2 != 0) return false;
    // Heuristic: count oriented edges on U after reduction-like state.
    // Real detection needs wing-edge orientation tracking.
    // For pipeline purposes we expose the alg and let caller decide;
    // here we use a simple edge-orientation proxy on outer edges.
    int flipped = 0;
    int mid = c.size() / 2;
    // Sample outer edge facelets on U
    if (c.get(U, 0, mid) != Color::U && c.get(U, 0, mid) != Color::D) ++flipped;
    if (c.get(U, mid, c.size()-1) != Color::U && c.get(U, mid, c.size()-1) != Color::D) ++flipped;
    if (c.get(U, c.size()-1, mid) != Color::U && c.get(U, c.size()-1, mid) != Color::D) ++flipped;
    if (c.get(U, mid, 0) != Color::U && c.get(U, mid, 0) != Color::D) ++flipped;
    // Odd number of "bad" orientations suggests OLL parity in reduction context
    return (flipped % 2) == 1;
}

bool ParityHandler::hasPLLParity(const Cube& c) {
    if (c.size() < 4 || c.size() % 2 != 0) return false;
    // Heuristic: after OLL, if two outer edges appear swapped vs centers
    // Simplified: always offer the alg when even-order and not solved
    return !c.isSolved();
}

std::vector<Move> ParityHandler::fixOLLParity(Cube& work) {
    // Standard 4x4 OLL parity (Uw2 style expressed with depth):
    // r2 B2 U2 l U2 r' U2 r U2 F2 r F2 l' B2 r2
    // Using depth-1 for inner R/L slices
    auto moves = parse("2R2 B2 U2 2L U2 2R' U2 2R U2 F2 2R F2 2L' B2 2R2");
    for (auto& m : moves) work.apply(m);
    return moves;
}

std::vector<Move> ParityHandler::fixPLLParity(Cube& work) {
    // Standard PLL parity (adjacent edge swap):
    // r2 U2 r2 Uw2 r2 u2
    auto moves = parse("2R2 U2 2R2 U2 2R2 U2");
    // More complete common alg:
    // Rw2 F2 U2 Rw2 R2 U2 F2 Rw2
    auto alt = parse("2R2 F2 U2 2R2 R2 U2 F2 2R2");
    for (auto& m : alt) work.apply(m);
    return alt;
}

std::vector<Move> ParityHandler::fix(Cube& work) {
    std::vector<Move> solution;
    if (work.size() < 4 || work.size() % 2 != 0) return solution;

    if (hasOLLParity(work)) {
        auto m = fixOLLParity(work);
        solution.insert(solution.end(), m.begin(), m.end());
    }
    if (hasPLLParity(work)) {
        auto m = fixPLLParity(work);
        solution.insert(solution.end(), m.begin(), m.end());
    }
    return solution;
}
