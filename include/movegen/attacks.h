#pragma once
#include <cstdint>
#include "move/move.h"
#include "board/board.h"
using u64 = uint64_t;
extern u64 knight_attacks[64];
extern u64 king_attacks[64];
extern u64 pawn_attacks[2][64];

void initAttackTables();

u64 maskKnightAttacks(int sq);
u64 maskKingAttacks(int sq);
u64 maskPawnAttacks(Square sq, Colour colour);