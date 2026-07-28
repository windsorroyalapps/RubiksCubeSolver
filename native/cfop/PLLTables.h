#pragma once

namespace PLL {

struct Alg {
    const char* name;
    const char* moves;
};

// Corner permutation (2-look)
inline const Alg CORNER_A_PERM = {"Aa", "R' F R' B2 R F' R' B2 R2"};
inline const Alg CORNER_E_PERM = {"E",  "R2 U R' U' y R U R' U' R U R' U' R U R' y' R U' R2"};

// Edge permutation (2-look)
inline const Alg EDGE_Ua = {"Ua", "R U' R U R U R U' R' U' R2"};
inline const Alg EDGE_Ub = {"Ub", "R2 U R U R' U' R' U' R' U R'"};
inline const Alg EDGE_H  = {"H",  "M2 U M2 U2 M2 U M2"};
inline const Alg EDGE_Z  = {"Z",  "M2 U M2 U M' U2 M2 U2 M' U2"};

// Full PLL highlights
inline const Alg T_PERM  = {"T",  "R U R' U' R' F R2 U' R' U' R U R' F'"};
inline const Alg Y_PERM  = {"Y",  "F R U' R' U' R U R' F' R U R' U' R' F R F'"};
inline const Alg J_PERM  = {"Ja", "R' U L' U2 R U' R' U2 R L"};
inline const Alg F_PERM  = {"F",  "R' U' F' R U R' U' R' F R2 U' R' U' R U R' U R"};

} // namespace PLL
