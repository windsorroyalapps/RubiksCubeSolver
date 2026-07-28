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
    return turns; // 0,1,2,3
}

Color Cube::get(int face, int row, int col) const {
    return facelets_[face][row][col];
}

void Cube::set(int face, int row, int col, Color c) {
    facelets_[face][row][col] = c;
}

void Cube::rotateFaceCW(int face) {
    // Rotate the face itself 90 degrees clockwise
    std::vector<std::vector<Color>> tmp = facelets_[face];
    for (int r = 0; r < n_; ++r) {
        for (int c = 0; c < n_; ++c) {
            facelets_[face][c][n_ - 1 - r] = tmp[r][c];
        }
    }
}

void Cube::rotateFace(int face, int turns) {
    turns = normalizeTurns(turns);
    for (int i = 0; i < turns; ++i) {
        rotateFaceCW(face);
    }
}

void Cube::cycleSides(int face, int depth, int turns) {
    turns = normalizeTurns(turns);
    if (turns == 0) return;

    // For each turn we cycle four side strips adjacent to the given face at the given depth.
    // depth 0 = outer layer (the face itself is rotated separately)

    auto cycle4 = [&](Color& a, Color& b, Color& c, Color& d) {
        Color t = a;
        a = d;
        d = c;
        c = b;
        b = t;
    };

    for (int t = 0; t < turns; ++t) {
        // We need to cycle the correct strips depending on which face is turning.
        // Indices: facelets_[face][row][col]

        if (face == U) {
            // Cycle top rows of F, R, B, L  (depth from outer)
            // U turn cycles the "depth" row of F -> R -> B -> L
            for (int i = 0; i < n_; ++i) {
                cycle4(
                    facelets_[F][depth][i],
                    facelets_[R][depth][i],
                    facelets_[B][depth][n_ - 1 - i],
                    facelets_[L][depth][n_ - 1 - i]
                );
            }
        } else if (face == D) {
            // Bottom rows of F, L, B, R
            int row = n_ - 1 - depth;
            for (int i = 0; i < n_; ++i) {
                cycle4(
                    facelets_[F][row][i],
                    facelets_[L][row][i],
                    facelets_[B][row][n_ - 1 - i],
                    facelets_[R][row][n_ - 1 - i]
                );
            }
        } else if (face == F) {
            // Bottom of U, left of R, top of D, right of L
            for (int i = 0; i < n_; ++i) {
                cycle4(
                    facelets_[U][n_ - 1 - depth][i],
                    facelets_[R][i][depth],
                    facelets_[D][depth][n_ - 1 - i],
                    facelets_[L][n_ - 1 - i][n_ - 1 - depth]
                );
            }
        } else if (face == B) {
            // Top of U, right of L, bottom of D, left of R
            for (int i = 0; i < n_; ++i) {
                cycle4(
                    facelets_[U][depth][i],
                    facelets_[L][i][depth],
                    facelets_[D][n_ - 1 - depth][n_ - 1 - i],
                    facelets_[R][n_ - 1 - i][n_ - 1 - depth]
                );
            }
        } else if (face == L) {
            // Left columns of U, F, D, B
            for (int i = 0; i < n_; ++i) {
                cycle4(
                    facelets_[U][i][depth],
                    facelets_[F][i][depth],
                    facelets_[D][i][depth],
                    facelets_[B][n_ - 1 - i][n_ - 1 - depth]
                );
            }
        } else if (face == R) {
            // Right columns of U, B, D, F
            for (int i = 0; i < n_; ++i) {
                cycle4(
                    facelets_[U][i][n_ - 1 - depth],
                    facelets_[B][n_ - 1 - i][depth],
                    facelets_[D][i][n_ - 1 - depth],
                    facelets_[F][i][n_ - 1 - depth]
                );
            }
        }
    }
}

void Cube::apply(const Move& m) {
    if (m.face < 0 || m.face > 5) return;
    if (m.depth < 0 || m.depth >= n_) return;

    int turns = m.turns;
    // Only rotate the outer face when depth == 0
    if (m.depth == 0) {
        rotateFace(m.face, turns);
    }
    cycleSides(m.face, m.depth, turns);
}

void Cube::apply(const std::vector<Move>& sequence) {
    for (const auto& m : sequence) {
        apply(m);
    }
}

bool Cube::isSolved() const {
    for (int f = 0; f < 6; ++f) {
        Color c = facelets_[f][0][0];
        for (int r = 0; r < n_; ++r) {
            for (int col = 0; col < n_; ++col) {
                if (facelets_[f][r][col] != c) return false;
            }
        }
    }
    return true;
}

std::string Cube::faceToString(int face) const {
    static const char* names = "UDFBLR";
    std::ostringstream oss;
    oss << names[face] << ":\n";
    for (int r = 0; r < n_; ++r) {
        for (int c = 0; c < n_; ++c) {
            oss << static_cast<int>(facelets_[face][r][c]) << ' ';
        }
        oss << '\n';
    }
    return oss.str();
}

std::string Cube::toString() const {
    std::ostringstream oss;
    oss << "Cube " << n_ << "x" << n_ << "x" << n_ << "\n";
    for (int f = 0; f < 6; ++f) {
        oss << faceToString(f) << '\n';
    }
    return oss.str();
}

void Cube::applyNotation(const std::string& notation) {
    // Very simple 3x3-only Singmaster parser: R, U, F, D, L, B, with ', 2
    // Example: "R U R' U'"
    if (n_ != 3) return; // only support classic notation for 3x3 here

    std::istringstream iss(notation);
    std::string token;
    while (iss >> token) {
        if (token.empty()) continue;
        char faceChar = token[0];
        int face = -1;
        switch (faceChar) {
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
