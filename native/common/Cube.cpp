#include "Cube.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

Cube::Cube(int size) : n_(size) {
    if (n_ < 2) throw std::invalid_argument("Cube size must be >= 2");
    for (int f = 0; f < 6; ++f) {
        facelets_[f].assign(n_, std::vector<Color>(n_, static_cast<Color>(f)));
    }
}

int Cube::normalizeTurns(int turns) {
    turns %= 4;
    if (turns < 0) turns += 4;
    return turns;
}

Color Cube::get(int face, int row, int col) const {
    return facelets_[face][row][col];
}

void Cube::set(int face, int row, int col, Color c) {
    facelets_[face][row][col] = c;
}

void Cube::rotateFaceCW(int face) {
    std::vector<std::vector<Color>> tmp = facelets_[face];
    for (int r = 0; r < n_; ++r)
        for (int c = 0; c < n_; ++c)
            facelets_[face][c][n_ - 1 - r] = tmp[r][c];
}

void Cube::rotateFace(int face, int turns) {
    turns = normalizeTurns(turns);
    for (int i = 0; i < turns; ++i) rotateFaceCW(face);
}

void Cube::cycleSides(int face, int depth, int turns) {
    turns = normalizeTurns(turns);
    if (turns == 0) return;

    auto cycle4 = [&](Color& a, Color& b, Color& c, Color& d) {
        Color t = a; a = d; d = c; c = b; b = t;
    };

    for (int t = 0; t < turns; ++t) {
        if (face == U) {
            for (int i = 0; i < n_; ++i)
                cycle4(facelets_[F][depth][i], facelets_[R][depth][i],
                       facelets_[B][depth][n_-1-i], facelets_[L][depth][n_-1-i]);
        } else if (face == D) {
            int row = n_ - 1 - depth;
            for (int i = 0; i < n_; ++i)
                cycle4(facelets_[F][row][i], facelets_[L][row][i],
                       facelets_[B][row][n_-1-i], facelets_[R][row][n_-1-i]);
        } else if (face == F) {
            for (int i = 0; i < n_; ++i)
                cycle4(facelets_[U][n_-1-depth][i], facelets_[R][i][depth],
                       facelets_[D][depth][n_-1-i], facelets_[L][n_-1-i][n_-1-depth]);
        } else if (face == B) {
            for (int i = 0; i < n_; ++i)
                cycle4(facelets_[U][depth][i], facelets_[L][i][depth],
                       facelets_[D][n_-1-depth][n_-1-i], facelets_[R][n_-1-i][n_-1-depth]);
        } else if (face == L) {
            for (int i = 0; i < n_; ++i)
                cycle4(facelets_[U][i][depth], facelets_[F][i][depth],
                       facelets_[D][i][depth], facelets_[B][n_-1-i][n_-1-depth]);
        } else if (face == R) {
            for (int i = 0; i < n_; ++i)
                cycle4(facelets_[U][i][n_-1-depth], facelets_[B][n_-1-i][depth],
                       facelets_[D][i][n_-1-depth], facelets_[F][i][n_-1-depth]);
        }
    }
}

void Cube::apply(const Move& m) {
    if (m.face < 0 || m.face > 5 || m.depth < 0 || m.depth >= n_) return;
    if (m.depth == 0) rotateFace(m.face, m.turns);
    cycleSides(m.face, m.depth, m.turns);
}

void Cube::apply(const std::vector<Move>& sequence) {
    for (const auto& m : sequence) apply(m);
}

bool Cube::isSolved() const {
    for (int f = 0; f < 6; ++f) {
        Color c = facelets_[f][0][0];
        for (int r = 0; r < n_; ++r)
            for (int col = 0; col < n_; ++col)
                if (facelets_[f][r][col] != c) return false;
    }
    return true;
}

std::string Cube::faceToString(int face) const {
    static const char* names = "UDFBLR";
    std::ostringstream oss;
    oss << names[face] << ":\n";
    for (int r = 0; r < n_; ++r) {
        for (int c = 0; c < n_; ++c)
            oss << static_cast<int>(facelets_[face][r][c]) << ' ';
        oss << '\n';
    }
    return oss.str();
}

std::string Cube::toString() const {
    std::ostringstream oss;
    oss << "Cube " << n_ << "x" << n_ << "x" << n_ << "\n";
    for (int f = 0; f < 6; ++f) oss << faceToString(f) << '\n';
    return oss.str();
}

void Cube::applyNotation(const std::string& notation) {
    if (n_ != 3) return;
    std::istringstream iss(notation);
    std::string token;
    while (iss >> token) {
        if (token.empty()) continue;
        int face = -1;
        switch (token[0]) {
            case 'U': face = U; break;
            case 'D': face = D; break;
            case 'F': face = F; break;
            case 'B': face = B; break;
            case 'L': face = L; break;
            case 'R': face = R; break;
            default: continue;
        }
        int turns = 1;
        if (token.size() > 1) {
            if (token[1] == '\'') turns = -1;
            else if (token[1] == '2') turns = 2;
        }
        apply(Move{face, 0, turns});
    }
}

// ---------- Solver helpers (3x3) ----------

void Cube::edgeCoords(Face f1, Face f2, int& faceA, int& rowA, int& colA,
                      int& faceB, int& rowB, int& colB) const {
    // Only valid for n==3. Map the 12 edge positions.
    // We always put the "primary" face first for consistency.
    auto mid = n_ / 2;

    // Normalize order so smaller face index is first when helpful
    struct Key { Face a, b; };
    // Exhaustive mapping for 3x3 edges
    if ((f1 == U && f2 == F) || (f1 == F && f2 == U)) {
        faceA = U; rowA = n_-1; colA = mid;
        faceB = F; rowB = 0;    colB = mid;
    } else if ((f1 == U && f2 == R) || (f1 == R && f2 == U)) {
        faceA = U; rowA = mid; colA = n_-1;
        faceB = R; rowB = 0;   colB = mid;
    } else if ((f1 == U && f2 == B) || (f1 == B && f2 == U)) {
        faceA = U; rowA = 0;   colA = mid;
        faceB = B; rowB = 0;   colB = mid;
    } else if ((f1 == U && f2 == L) || (f1 == L && f2 == U)) {
        faceA = U; rowA = mid; colA = 0;
        faceB = L; rowB = 0;   colB = mid;
    } else if ((f1 == D && f2 == F) || (f1 == F && f2 == D)) {
        faceA = D; rowA = 0;   colA = mid;
        faceB = F; rowB = n_-1; colB = mid;
    } else if ((f1 == D && f2 == R) || (f1 == R && f2 == D)) {
        faceA = D; rowA = mid; colA = n_-1;
        faceB = R; rowB = n_-1; colB = mid;
    } else if ((f1 == D && f2 == B) || (f1 == B && f2 == D)) {
        faceA = D; rowA = n_-1; colA = mid;
        faceB = B; rowB = n_-1; colB = mid;
    } else if ((f1 == D && f2 == L) || (f1 == L && f2 == D)) {
        faceA = D; rowA = mid; colA = 0;
        faceB = L; rowB = n_-1; colB = mid;
    } else if ((f1 == F && f2 == R) || (f1 == R && f2 == F)) {
        faceA = F; rowA = mid; colA = n_-1;
        faceB = R; rowB = mid; colB = 0;
    } else if ((f1 == R && f2 == B) || (f1 == B && f2 == R)) {
        faceA = R; rowA = mid; colA = n_-1;
        faceB = B; rowB = mid; colB = 0;
    } else if ((f1 == B && f2 == L) || (f1 == L && f2 == B)) {
        faceA = B; rowA = mid; colA = n_-1;
        faceB = L; rowB = mid; colB = 0;
    } else { // F-L
        faceA = F; rowA = mid; colA = 0;
        faceB = L; rowB = mid; colB = n_-1;
    }
}

std::pair<Color, Color> Cube::edgeColors(Face f1, Face f2) const {
    int fa, ra, ca, fb, rb, cb;
    edgeCoords(f1, f2, fa, ra, ca, fb, rb, cb);
    return {facelets_[fa][ra][ca], facelets_[fb][rb][cb]};
}

std::array<Color, 3> Cube::cornerColors(Face f1, Face f2, Face f3) const {
    // Simplified: read the three facelets at the corner of f1/f2/f3
    // For 3x3 the corner is uniquely determined.
    int mid = n_ / 2;
    // We use a practical approach: sample the three faces at their corner coords.
    // This is a best-effort for the solver; full geometric mapping can be refined.
    auto cornerOn = [&](Face f, Face a, Face b) -> Color {
        // Return the facelet of face f that is adjacent to both a and b
        if (f == U) {
            if ((a == F || b == F) && (a == R || b == R)) return facelets_[U][n_-1][n_-1];
            if ((a == F || b == F) && (a == L || b == L)) return facelets_[U][n_-1][0];
            if ((a == B || b == B) && (a == R || b == R)) return facelets_[U][0][n_-1];
            if ((a == B || b == B) && (a == L || b == L)) return facelets_[U][0][0];
        } else if (f == D) {
            if ((a == F || b == F) && (a == R || b == R)) return facelets_[D][0][n_-1];
            if ((a == F || b == F) && (a == L || b == L)) return facelets_[D][0][0];
            if ((a == B || b == B) && (a == R || b == R)) return facelets_[D][n_-1][n_-1];
            if ((a == B || b == B) && (a == L || b == L)) return facelets_[D][n_-1][0];
        } else if (f == F) {
            if ((a == U || b == U) && (a == R || b == R)) return facelets_[F][0][n_-1];
            if ((a == U || b == U) && (a == L || b == L)) return facelets_[F][0][0];
            if ((a == D || b == D) && (a == R || b == R)) return facelets_[F][n_-1][n_-1];
            if ((a == D || b == D) && (a == L || b == L)) return facelets_[F][n_-1][0];
        } else if (f == B) {
            if ((a == U || b == U) && (a == R || b == R)) return facelets_[B][0][0];
            if ((a == U || b == U) && (a == L || b == L)) return facelets_[B][0][n_-1];
            if ((a == D || b == D) && (a == R || b == R)) return facelets_[B][n_-1][0];
            if ((a == D || b == D) && (a == L || b == L)) return facelets_[B][n_-1][n_-1];
        } else if (f == L) {
            if ((a == U || b == U) && (a == F || b == F)) return facelets_[L][0][n_-1];
            if ((a == U || b == U) && (a == B || b == B)) return facelets_[L][0][0];
            if ((a == D || b == D) && (a == F || b == F)) return facelets_[L][n_-1][n_-1];
            if ((a == D || b == D) && (a == B || b == B)) return facelets_[L][n_-1][0];
        } else { // R
            if ((a == U || b == U) && (a == F || b == F)) return facelets_[R][0][0];
            if ((a == U || b == U) && (a == B || b == B)) return facelets_[R][0][n_-1];
            if ((a == D || b == D) && (a == F || b == F)) return facelets_[R][n_-1][0];
            if ((a == D || b == D) && (a == B || b == B)) return facelets_[R][n_-1][n_-1];
        }
        return facelets_[f][mid][mid]; // fallback center
    };

    return {
        cornerOn(f1, f2, f3),
        cornerOn(f2, f1, f3),
        cornerOn(f3, f1, f2)
    };
}

bool Cube::isWhiteCrossSolved() const {
    if (n_ != 3) return false;
    // White is Color::D (we treat D as white/bottom for beginner method)
    // Cross on D: four edges DF, DR, DB, DL must have D color on D face
    // and matching side colors.
    auto check = [&](Face side, Color expectedSide) {
        auto [c1, c2] = edgeColors(D, side);
        // one of them must be D (white), the other the side color
        bool hasWhite = (c1 == Color::D || c2 == Color::D);
        bool hasSide  = (c1 == expectedSide || c2 == expectedSide);
        // Also the D facelet itself should be white
        int mid = n_ / 2;
        Color onD;
        if (side == F) onD = facelets_[D][0][mid];
        else if (side == R) onD = facelets_[D][mid][n_-1];
        else if (side == B) onD = facelets_[D][n_-1][mid];
        else onD = facelets_[D][mid][0];
        return hasWhite && hasSide && onD == Color::D;
    };
    return check(F, Color::F) && check(R, Color::R) &&
           check(B, Color::B) && check(L, Color::L);
}

bool Cube::isFirstLayerSolved() const {
    if (n_ != 3) return false;
    if (!isWhiteCrossSolved()) return false;
    // Check four white corners on D
    for (int r = 0; r < n_; r += n_-1)
        for (int c = 0; c < n_; c += n_-1)
            if (facelets_[D][r][c] != Color::D) return false;
    return true;
}
