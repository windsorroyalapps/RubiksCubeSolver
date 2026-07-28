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

    // After reduction, OLL parity = odd number of flipped wing-pairs
    // on the last layer relative to a 3x3 model.
    // Proxy: count outer-layer edges on U whose adjacent side color
    // does not match the side center (misoriented dedge).
    int n = c.size();
    int mid = n / 2;
    int bad = 0;

    // UF edge: U facelet at (n-1, mid), F facelet at (0, mid)
    Color uUF = c.get(U, n - 1, mid);
    Color fUF = c.get(F, 0, mid);
    if (!((uUF == Color::U || uUF == Color::D) || (fUF == Color::U || fUF == Color::D)))
        ++bad;

    Color uUR = c.get(U, mid, n - 1);
    Color rUR = c.get(R, 0, mid);
    if (!((uUR == Color::U || uUR == Color::D) || (rUR == Color::U || rUR == Color::D)))
        ++bad;

    Color uUB = c.get(U, 0, mid);
    Color bUB = c.get(B, 0, mid);
    if (!((uUB == Color::U || uUB == Color::D) || (bUB == Color::U || bUB == Color::D)))
        ++bad;

    Color uUL = c.get(U, mid, 0);
    Color lUL = c.get(L, 0, mid);
    if (!((uUL == Color::U || uUL == Color::D) || (lUL == Color::U || lUL == Color::D)))
        ++bad;

    // OLL parity presents as a single "flipped" edge in 3x3 terms => odd count
    return (bad % 2) == 1;
}

bool ParityHandler::hasPLLParity(const Cube& c) {
    if (c.size() < 4 || (c.size() % 2) != 0) return false;

    // PLL parity = odd permutation of the 12 dedges (two edges swapped).
    // Proxy after OLL: check whether opposite edge colors form consistent pairs.
    int n = c.size();
    int mid = n / 2;

    // Side colors of the four U edges
    Color sUF = c.get(F, 0, mid);
    Color sUR = c.get(R, 0, mid);
    Color sUB = c.get(B, 0, mid);
    Color sUL = c.get(L, 0, mid);

    // In a solved-or-even-perm state, opposite sides should not both be "adjacent-only" mismatches
    // Count cycles of length 2 among the four
    int matches = 0;
    if (sUF == Color::F) ++matches;
    if (sUR == Color::R) ++matches;
    if (sUB == Color::B) ++matches;
    if (sUL == Color::L) ++matches;

    // 4 = solved edges, 1 or 3 often indicates odd perm / parity case
    // 0 or 2 can be even perms (H, Z, U-perms etc.)
    return matches == 1 || matches == 3;
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
