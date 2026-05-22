#include "attacks.h"
#include "masks.h"
u64 maskKnightAttacks(int sq)
{
    u64 attacks = 0ULL;
    u64 bit = 1ULL << sq;

    if ((bit << 17) & ~FILE_A) attacks |= (bit << 17);
    if ((bit << 15) & ~FILE_H) attacks |= (bit << 15);
    if ((bit << 10) & ~(FILE_A | FILE_B)) attacks |= (bit << 10);
    if ((bit << 6)  & ~(FILE_H | FILE_G)) attacks |= (bit << 6);

    if ((bit >> 17) & ~FILE_H) attacks |= (bit >> 17);
    if ((bit >> 15) & ~FILE_A) attacks |= (bit >> 15);
    if ((bit >> 10) & ~(FILE_H | FILE_G)) attacks |= (bit >> 10);
    if ((bit >> 6)  & ~(FILE_A | FILE_B)) attacks |= (bit >> 6);

    return attacks;
}

u64 maskKingAttacks(int sq)
{
    u64 attacks = 0ULL;
    u64 bit = 1ULL << sq;

    attacks |= (bit & ~FILE_H) << 1;
    attacks |= (bit & ~FILE_A) >> 1;

    attacks |= bit << 8;
    attacks |= bit >> 8;

    attacks |= (bit & ~FILE_H) << 9;
    attacks |= (bit & ~FILE_A) << 7;

    attacks |= (bit & ~FILE_H) >> 7;
    attacks |= (bit & ~FILE_A) >> 9;

    return attacks;
}

void initAttackTables(){
    for(int i = 0; i<64; i++){
        knight_attacks[i] = maskKnightAttacks(i);
        king_attacks[i] = maskKingAttacks(i);
    }
}