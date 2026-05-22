#include "bitboard.h"
#include "masks.h"
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