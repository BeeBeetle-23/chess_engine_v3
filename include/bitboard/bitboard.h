#pragma once
#include <cstdint>
using u64 = uint64_t;
inline int pop_lsb(u64 &bb)
{
    int sq = __builtin_ctzll(bb);
    bb &= bb - 1;
    return sq;
}
inline u64 setBitZero(u64 num, int pos){
    return (num & ~(1ULL<<pos));
}
inline u64 setBitOne(u64 num, int pos){
    return (num | (1Ull<<pos));
}
inline bool is_in_check(const Board &board){
    Colour us = board.side_to_move;
    Colour them = static_cast<Colour>(board.side_to_move^1);
    if(board.isSquareAttacked(board.king_square[us],them)) return true;
    return false;
}