#include "StageCap.h"

static Move invertMove(const Move& m) {
    Move inv = m;
    if (m.turns == 1) inv.turns = 3;
    else if (m.turns == 3) inv.turns = 1;
    // 180 stays 180
    return inv;
}

std::vector<Move> StageCap::eightMoveCommutator(const Move& a, const Move& b) {
    // A B A' B' then repeat once → 8 quarter turns (typical center/wing commutator budget).
    std::vector<Move> seq;
    seq.reserve(8);
    Move ap = invertMove(a);
    Move bp = invertMove(b);
    seq.push_back(a);
    seq.push_back(b);
    seq.push_back(ap);
    seq.push_back(bp);
    seq.push_back(a);
    seq.push_back(b);
    seq.push_back(ap);
    seq.push_back(bp);
    return seq;
}

std::vector<Move> StageCap::capToBudget(const std::vector<Move>& seq, int budget) {
    if (budget < 0) budget = 0;
    if (static_cast<int>(seq.size()) <= budget) return seq;
    return std::vector<Move>(seq.begin(), seq.begin() + budget);
}
