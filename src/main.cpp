#include <iostream>
#include <cstdint>
#include "move/move.h"
#include "board/board.h"
#include "movegen/attacks.h"
#include "movegen/generateattacks.h"
#include "bitboard/perft.h"
#include "search/search.h"
#include <array>
#include <vector>
using u64 = uint64_t;
int main(){
    Board board;
    initAttackTables();
    board.clearBoard();
    board.Parse_FEN("1k5r/pp6/4q3/1N6/8/P4Q2/1PP5/1K6 w - - 0 1");
    Move best = findBestMove(board,6);
    best.print_into_algebraic(board);
    return 0;
}