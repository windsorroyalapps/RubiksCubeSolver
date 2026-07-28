#include "CFOPSolver.h"
#include "OLLTables.h"
#include "PLLTables.h"

#include <sstream>

Move CFOPSolver::parseToken(const std::string& token) {
    if (token.empty()) return Move{0, 0, 0};
    int face = 0;
    switch (token[0]) {
        case 'U': face = U; break;
        case 'D': face = D; break;
        case 'F': face = F; break;
        case 'B': face = B; break;
        case 'L': face = L; break;
        case 'R': face = R; break;
        default: return Move{0, 0, 0};
    }
    int turns = 1;
    if (token.size() > 1) {
        if (token[1] == '\'') turns = -1;
        else if (token[1] == '2') turns = 2;
    }
    return Move{face, 0, turns};
}

std::vector<Move> CFOPSolver::parseSequence(const std::string& notation) {
    std::vector<Move> result;
    std::istringstream iss(notation);
    std::string token;
    while (iss >> token)
        if (!token.empty()) result.push_back(parseToken(token));
    return result;
}

void CFOPSolver::appendSeq(Cube& work, std::vector<Move>& out, const std::string& seq) {
    auto m = parseSequence(seq);
    for (auto& x : m) {
        work.apply(x);
        out.push_back(x);
    }
}

// ---------------------------------------------------------------------------
// Recognition
// ---------------------------------------------------------------------------

int CFOPSolver::countYellowEdgesOnU(const Cube& c) {
    // Yellow = Color::U in our model (top face color when solved)
    int mid = 1; // 3x3
    int count = 0;
    if (c.get(U, 0, mid) == Color::U) ++count; // UB
    if (c.get(U, mid, 2) == Color::U) ++count; // UR
    if (c.get(U, 2, mid) == Color::U) ++count; // UF
    if (c.get(U, mid, 0) == Color::U) ++count; // UL
    return count;
}

int CFOPSolver::countYellowCornersOnU(const Cube& c) {
    int count = 0;
    if (c.get(U, 0, 0) == Color::U) ++count;
    if (c.get(U, 0, 2) == Color::U) ++count;
    if (c.get(U, 2, 0) == Color::U) ++count;
    if (c.get(U, 2, 2) == Color::U) ++count;
    return count;
}

bool CFOPSolver::isYellowCross(const Cube& c) {
    return countYellowEdgesOnU(c) == 4;
}

// ---------------------------------------------------------------------------
// First two layers (beginner)
// ---------------------------------------------------------------------------

std::vector<Move> CFOPSolver::solveWhiteCross(Cube& work) {
    std::vector<Move> moves;
    if (work.size() != 3) return moves;

    for (int pass = 0; pass < 8 && !work.isWhiteCrossSolved(); ++pass) {
        appendSeq(work, moves, "F' U F");
        appendSeq(work, moves, "U");
        appendSeq(work, moves, "F2");
        appendSeq(work, moves, "R' U R U R2");
        appendSeq(work, moves, "B' U B U B2");
        appendSeq(work, moves, "L' U L U L2");
    }
    return moves;
}

std::vector<Move> CFOPSolver::solveWhiteCorners(Cube& work) {
    std::vector<Move> moves;
    for (int i = 0; i < 16; ++i) {
        if (work.isFirstLayerSolved()) break;
        appendSeq(work, moves, "R U R' U'");
        if (i % 4 == 3) appendSeq(work, moves, "U");
    }
    return moves;
}

std::vector<Move> CFOPSolver::solveMiddleEdges(Cube& work) {
    std::vector<Move> moves;
    for (int i = 0; i < 6; ++i) {
        appendSeq(work, moves, "U R U' R' U' F' U F");
        appendSeq(work, moves, "U");
        appendSeq(work, moves, "U' L' U L U F U' F'");
        appendSeq(work, moves, "U");
    }
    return moves;
}

// ---------------------------------------------------------------------------
// Pattern-aware last layer
// ---------------------------------------------------------------------------

std::vector<Move> CFOPSolver::solveYellowCross(Cube& work) {
    std::vector<Move> moves;

    for (int attempt = 0; attempt < 4; ++attempt) {
        int edges = countYellowEdgesOnU(work);
        if (edges == 4) break; // already cross

        if (edges == 0) {
            // Dot
            appendSeq(work, moves, OLL::EDGE_DOT.moves);
        } else if (edges == 2) {
            // Could be L or Line — try Line alg first, then L
            // Check if opposite edges (line) vs adjacent (L)
            bool uf = work.get(U, 2, 1) == Color::U;
            bool ub = work.get(U, 0, 1) == Color::U;
            bool ul = work.get(U, 1, 0) == Color::U;
            bool ur = work.get(U, 1, 2) == Color::U;

            if ((uf && ub) || (ul && ur)) {
                // Line — orient so line is horizontal
                if (ul && ur) appendSeq(work, moves, "U");
                appendSeq(work, moves, OLL::EDGE_LINE.moves);
            } else {
                // L-shape — put L in back-left
                appendSeq(work, moves, OLL::EDGE_L.moves);
            }
        } else {
            // Fallback
            appendSeq(work, moves, OLL::EDGE_LINE.moves);
        }
        if (!isYellowCross(work)) appendSeq(work, moves, "U");
    }
    return moves;
}

std::vector<Move> CFOPSolver::orientYellowCorners(Cube& work) {
    std::vector<Move> moves;

    for (int attempt = 0; attempt < 6; ++attempt) {
        int corners = countYellowCornersOnU(work);
        if (corners == 4) break;

        if (corners == 1) {
            // Sune or AntiSune — try Sune, rotate if needed
            appendSeq(work, moves, OLL::CORNER_SUNE.moves);
        } else if (corners == 0) {
            // Pi / H / other — try Pi then H
            appendSeq(work, moves, OLL::CORNER_PI.moves);
        } else if (corners == 2) {
            // Headlights / T / Bowtie
            appendSeq(work, moves, OLL::CORNER_HEADLIGHTS.moves);
        } else {
            appendSeq(work, moves, OLL::CORNER_SUNE.moves);
        }
        if (countYellowCornersOnU(work) < 4) appendSeq(work, moves, "U");
    }
    return moves;
}

std::vector<Move> CFOPSolver::permuteYellowCorners(Cube& work) {
    std::vector<Move> moves;
    // A-perm cycles; try a few orientations
    for (int i = 0; i < 4; ++i) {
        appendSeq(work, moves, PLL::CORNER_A_PERM.moves);
        appendSeq(work, moves, "U");
    }
    return moves;
}

std::vector<Move> CFOPSolver::permuteYellowEdges(Cube& work) {
    std::vector<Move> moves;
    for (int i = 0; i < 3; ++i) {
        appendSeq(work, moves, PLL::EDGE_Ua.moves);
        if (work.isSolved()) break;
        appendSeq(work, moves, "U");
        appendSeq(work, moves, PLL::EDGE_Ub.moves);
        if (work.isSolved()) break;
        appendSeq(work, moves, "U");
    }
    // H-perm as final cleanup if still not solved
    if (!work.isSolved()) {
        appendSeq(work, moves, PLL::EDGE_H.moves);
    }
    return moves;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

std::vector<Move> CFOPSolver::solve(const Cube& cube) {
    if (cube.size() != 3) return {};

    Cube work = cube;
    std::vector<Move> solution;

    auto collect = [&](const std::vector<Move>& stage) {
        for (const auto& m : stage) solution.push_back(m);
    };

    collect(solveWhiteCross(work));
    collect(solveWhiteCorners(work));
    collect(solveMiddleEdges(work));
    collect(solveYellowCross(work));
    collect(orientYellowCorners(work));
    collect(permuteYellowCorners(work));
    collect(permuteYellowEdges(work));

    return solution;
}

std::string CFOPSolver::solveToNotation(const Cube& cube) {
    auto moves = solve(cube);
    static const char* faces = "UDFBLR";
    std::ostringstream oss;
    for (size_t i = 0; i < moves.size(); ++i) {
        const auto& m = moves[i];
        oss << faces[m.face];
        if (m.turns == 2) oss << '2';
        else if (m.turns == -1 || m.turns == 3) oss << '\'';
        if (i + 1 < moves.size()) oss << ' ';
    }
    return oss.str();
}
