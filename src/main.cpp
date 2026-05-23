#include <iostream>
#include <cstdint>
#include "move/move.h"
#include "board/board.h"
#include "movegen/attacks.h"
#include "movegen/generateattacks.h"
#include "bitboard/perft.h"
#include <array>
#include <vector>
using u64 = uint64_t;
int main()
{
    initAttackTables();
    Board board;
    board.Parse_FEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    perft_divide(board,5);
    return 0;
}