#include "ParityHandler.h"

#include <sstream>

static std::vector<Move> parse(const std::string& notation) {
    std::vector<Move> result;
    std::istringstream iss(notation);
    std::string token;
    while (iss >> token) {
        if (token.empty()) continue;
        int depth = 0;
        size_t pos = 0;
        if (token[0] >= '2' && token[0] <= '9') {
            depth = token[0] - '1';
            pos = 1;
        }
        if (pos >= token.size()) continue;
        int face = -1;
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

static void applyAll(Cube& work, const std::vector<Move>& moves) {
    for (const auto& m : moves) work.apply(m);
}

bool ParityHandler::hasOLLParity(const Cube& c) {
    if (c.size() < 4 || (c.size() % 2) != 0) return false;

    // After reduction, OLL parity = odd number of flipped wing-pairs on last layer.
    // Sample multiple wing depths (not only mid) so n>4 is covered better.
    int n = c.size();
    int bad = 0;

    auto checkEdge = [&](int faceU_row, int faceU_col, int sideFace, int sideRow, int sideCol) {
        Color u = c.get(U, faceU_row, faceU_col);
        Color s = c.get(sideFace, sideRow, sideCol);
        // Oriented if U/D color sits on U face, or side is U/D (rare after centers)
        bool oriented = (u == Color::U || u == Color::D) ||
                        (s == Color::U || s == Color::D);
        if (!oriented) ++bad;
    };

    // Depths to sample: mid always; also 1 and n-2 when they differ
    int depths[3];
    int nd = 0;
    depths[nd++] = n / 2;
    if (n > 4) {
        depths[nd++] = 1;
        depths[nd++] = n - 2;
    }

    for (int i = 0; i < nd; ++i) {
        int d = depths[i];
        // UF
        checkEdge(n - 1, d, F, 0, d);
        // UR
        checkEdge(d, n - 1, R, 0, d);
        // UB
        checkEdge(0, d, B, 0, d);
        // UL
        checkEdge(d, 0, L, 0, d);
    }

    // OLL parity presents as a single "flipped" edge in 3x3 terms => odd count
    return (bad % 2) == 1;
}

bool ParityHandler::hasPLLParity(const Cube& c) {
    if (c.size() < 4 || (c.size() % 2) != 0) return false;

    // PLL parity = odd permutation of the 12 dedges (two edges swapped).
    // Multi-depth side-color match count for robustness on n>4.
    int n = c.size();
    int matches = 0;
    int samples = 0;

    int depths[3];
    int nd = 0;
    depths[nd++] = n / 2;
    if (n > 4) {
        depths[nd++] = 1;
        depths[nd++] = n - 2;
    }

    for (int i = 0; i < nd; ++i) {
        int d = depths[i];
        if (c.get(F, 0, d) == Color::F) ++matches;
        if (c.get(R, 0, d) == Color::R) ++matches;
        if (c.get(B, 0, d) == Color::B) ++matches;
        if (c.get(L, 0, d) == Color::L) ++matches;
        samples += 4;
    }

    // Normalize: expected full match = samples. Odd residual suggests parity.
    int residual = samples - matches;
    // Classic mid-only heuristic: matches == 1 or 3 on 4 samples.
    // For multi-sample, treat odd residual in the low band as parity-like.
    if (nd == 1)
        return matches == 1 || matches == 3;
    return (residual % 2 == 1) && residual > 0 && residual < samples;
}

std::vector<Move> ParityHandler::fixOLLParity(Cube& work) {
    // Widely used 4x4 OLL parity (inner r/l):
    // r2 B2 U2 l U2 r' U2 r U2 F2 r F2 l' B2 r2
    auto moves = parse("2R2 B2 U2 2L U2 2R' U2 2R U2 F2 2R F2 2L' B2 2R2");
    applyAll(work, moves);
    return moves;
}

std::vector<Move> ParityHandler::fixPLLParity(Cube& work) {
    // Common PLL parity (opposite edge swap style):
    // Rw2 F2 U2 Rw2 R2 U2 F2 Rw2
    auto moves = parse("2R2 F2 U2 2R2 R2 U2 F2 2R2");
    applyAll(work, moves);
    return moves;
}

std::vector<Move> ParityHandler::fix(Cube& work) {
    std::vector<Move> solution;
    if (work.size() < 4 || (work.size() % 2) != 0) return solution;

    // OLL parity first, then PLL parity (standard order)
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
