#include "movegen/attacks.h"
#include "bitboard/masks.h"
#include "move/move.h"
#include "board/board.h"
#include <cassert>
u64 king_attacks[64];
u64 knight_attacks[64];
u64 pawn_attacks[2][64];
u64 maskKnightAttacks(int sq)
{
    assert(sq >= 0 && sq < 64 && "square out of range");

    u64 attacks = 0ULL;
    u64 bit = 1ULL << sq;

    // +2 rank, +1 file: wraps off FILE_H
    if (bit & ~FILE_H)          attacks |= (bit << 17);

    // +2 rank, -1 file: wraps off FILE_A
    if (bit & ~FILE_A)          attacks |= (bit << 15);

    // +1 rank, +2 file: wraps off FILE_G or FILE_H
    if (bit & ~(FILE_G | FILE_H)) attacks |= (bit << 10);

    // +1 rank, -2 file: wraps off FILE_A or FILE_B
    if (bit & ~(FILE_A | FILE_B)) attacks |= (bit << 6);

    // -2 rank, -1 file: wraps off FILE_A
    if (bit & ~FILE_A)          attacks |= (bit >> 17);

    // -2 rank, +1 file: wraps off FILE_H
    if (bit & ~FILE_H)          attacks |= (bit >> 15);

    // -1 rank, -2 file: wraps off FILE_A or FILE_B
    if (bit & ~(FILE_A | FILE_B)) attacks |= (bit >> 10);

    // -1 rank, +2 file: wraps off FILE_G or FILE_H
    if (bit & ~(FILE_G | FILE_H)) attacks |= (bit >> 6);

    return attacks;
}

u64 maskKingAttacks(int sq)
{
    u64 attacks = 0ULL;

    u64 bitboard = 1ULL << sq;

    // north
    attacks |= (bitboard << 8);

    // south
    attacks |= (bitboard >> 8);

    // east
    attacks |= (bitboard & ~FILE_H) << 1;

    // west
    attacks |= (bitboard & ~FILE_A) >> 1;

    // north-east
    attacks |= (bitboard & ~FILE_H) << 9;

    // north-west
    attacks |= (bitboard & ~FILE_A) << 7;

    // south-east
    attacks |= (bitboard & ~FILE_H) >> 7;

    // south-west
    attacks |= (bitboard & ~FILE_A) >> 9;

    return attacks;
}

u64 maskPawnAttacks(Square sq, Colour side)
{
    u64 attacks = 0ULL;
    u64 bitboard = 1ULL << sq;

    if (side == WHITE)
    {
        if (bitboard & ~FILE_A)
            attacks |= (bitboard << 7);

        if (bitboard & ~FILE_H)
            attacks |= (bitboard << 9);
    }
    else
    {
        if (bitboard & ~FILE_A)
            attacks |= (bitboard >> 9);

        if (bitboard & ~FILE_H)
            attacks |= (bitboard >> 7);
    }

    return attacks;
}

void initAttackTables()
{
    for (int i = 0; i < 64; i++)
    {
        knight_attacks[i] = maskKnightAttacks(i);
        king_attacks[i] = maskKingAttacks(i);

        pawn_attacks[WHITE][i] = maskPawnAttacks((Square)i, WHITE);
        pawn_attacks[BLACK][i] = maskPawnAttacks((Square)i, BLACK);
    }
}