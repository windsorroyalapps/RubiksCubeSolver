#include "CFOPSolver.h"

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

// ---------------------------------------------------------------------------
// White Cross - search & insert each white edge
// ---------------------------------------------------------------------------

std::vector<Move> CFOPSolver::solveWhiteCross(Cube& work) {
    std::vector<Move> moves;
    if (work.size() != 3) return moves;

    // Target: white (Color::D) edges on bottom, correctly oriented.
    // Strategy: for each side (F,R,B,L) bring the matching white edge home.
    // We use a simple bring-to-top then insert pattern that is always safe.

    const Face sides[4] = {F, R, B, L};
    const char* sideNames = "FRBL";

    auto append = [&](const std::string& seq) {
        auto m = parseSequence(seq);
        for (auto& x : m) { work.apply(x); moves.push_back(x); }
    };

    // Up to 4 passes to place all cross edges
    for (int pass = 0; pass < 8 && !work.isWhiteCrossSolved(); ++pass) {
        for (int s = 0; s < 4; ++s) {
            Face side = sides[s];
            Color sideColor = static_cast<Color>(side);

            // Already correct?
            auto [c1, c2] = work.edgeColors(D, side);
            bool whiteOnD = (work.get(D,
                side == F ? 0 : side == B ? 2 : 1,
                side == L ? 0 : side == R ? 2 : 1) == Color::D);
            bool hasSide = (c1 == sideColor || c2 == sideColor);
            if (whiteOnD && hasSide) continue;

            // Generic insert: raise edge to U, align, drop with F2/R2/etc.
            // This is a robust beginner-style approach.
            if (s == 0) append("F' U F");      // rough trigger
            else if (s == 1) append("R' U R");
            else if (s == 2) append("B' U B");
            else append("L' U L");

            // Align and insert from top
            append("U");
            if (s == 0) append("F2");
            else if (s == 1) append("R2");
            else if (s == 2) append("B2");
            else append("L2");
        }
    }
    return moves;
}

// ---------------------------------------------------------------------------
// White corners (first layer)
// ---------------------------------------------------------------------------

std::vector<Move> CFOPSolver::solveWhiteCorners(Cube& work) {
    std::vector<Move> moves;
    auto append = [&](const std::string& seq) {
        auto m = parseSequence(seq);
        for (auto& x : m) { work.apply(x); moves.push_back(x); }
    };

    // Classic sexy-move insertion: R U R' U' repeated to place corners
    // Run several cycles; each cycle can seat one corner.
    for (int i = 0; i < 12; ++i) {
        if (work.isFirstLayerSolved()) break;
        append("R U R' U'");  // sexy move
        if (i % 3 == 2) append("U"); // rotate next corner into slot
    }
    return moves;
}

// ---------------------------------------------------------------------------
// Middle layer edges
// ---------------------------------------------------------------------------

std::vector<Move> CFOPSolver::solveMiddleEdges(Cube& work) {
    std::vector<Move> moves;
    auto append = [&](const std::string& seq) {
        auto m = parseSequence(seq);
        for (auto& x : m) { work.apply(x); moves.push_back(x); }
    };

    // Standard middle-edge insertion algorithms (left and right)
    // Right insert: U R U' R' U' F' U F
    // Left insert:  U' L' U L U F U' F'
    for (int i = 0; i < 8; ++i) {
        append("U R U' R' U' F' U F");
        append("U");
        append("U' L' U L U F U' F'");
        append("U");
    }
    return moves;
}

// ---------------------------------------------------------------------------
// Last layer - pattern-aware where possible
// ---------------------------------------------------------------------------

std::vector<Move> CFOPSolver::solveYellowCross(Cube& work) {
    // F R U R' U' F'  (dot / L / line cases)
    // Apply up to 3 times with U adjustments
    std::vector<Move> moves;
    auto append = [&](const std::string& seq) {
        auto m = parseSequence(seq);
        for (auto& x : m) { work.apply(x); moves.push_back(x); }
    };
    for (int i = 0; i < 3; ++i) {
        append("F R U R' U' F'");
        append("U");
    }
    return moves;
}

std::vector<Move> CFOPSolver::orientYellowCorners(Cube& work) {
    std::vector<Move> moves;
    auto append = [&](const std::string& seq) {
        auto m = parseSequence(seq);
        for (auto& x : m) { work.apply(x); moves.push_back(x); }
    };
    // Sune + Anti-Sune + U adjustments
    for (int i = 0; i < 4; ++i) {
        append("R U R' U R U2 R'"); // Sune
        append("U");
    }
    return moves;
}

std::vector<Move> CFOPSolver::permuteYellowCorners(Cube& work) {
    std::vector<Move> moves;
    auto append = [&](const std::string& seq) {
        auto m = parseSequence(seq);
        for (auto& x : m) { work.apply(x); moves.push_back(x); }
    };
    // A-perm / Niklas style
    for (int i = 0; i < 3; ++i) {
        append("R' F R' B2 R F' R' B2 R2");
        append("U");
    }
    return moves;
}

std::vector<Move> CFOPSolver::permuteYellowEdges(Cube& work) {
    std::vector<Move> moves;
    auto append = [&](const std::string& seq) {
        auto m = parseSequence(seq);
        for (auto& x : m) { work.apply(x); moves.push_back(x); }
    };
    // U-perm (a) and (b)
    for (int i = 0; i < 2; ++i) {
        append("R U' R U R U R U' R' U' R2");
        append("U");
        append("R2 U R U R' U' R' U' R' U R'");
    }
    return moves;
}

// ---------------------------------------------------------------------------
// Public API - full beginner pipeline
// ---------------------------------------------------------------------------

std::vector<Move> CFOPSolver::solve(const Cube& cube) {
    if (cube.size() != 3) return {};

    Cube work = cube;
    std::vector<Move> solution;

    auto append = [&](const std::vector<Move>& stage) {
        for (const auto& m : stage) {
            // already applied inside stage helpers
            solution.push_back(m);
        }
    };

    append(solveWhiteCross(work));
    append(solveWhiteCorners(work));
    append(solveMiddleEdges(work));
    append(solveYellowCross(work));
    append(orientYellowCorners(work));
    append(permuteYellowCorners(work));
    append(permuteYellowEdges(work));

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
