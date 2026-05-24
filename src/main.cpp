#include <iostream>
#include <cstdint>
#include "move/move.h"
#include "board/board.h"
#include "movegen/attacks.h"
#include "movegen/generateattacks.h"
#include "bitboard/perft.h"
#include "evaluation/evaluation.h"
#include <array>
#include <vector>
using u64 = uint64_t;
int main(){
    Board board;
    board.clearBoard();
    board.Parse_FEN("r1bqkbnr/4pppp/p1pp4/8/4P3/2N5/PPP2PPP/R1BQKB1R w KQkq - 0 7");
    std::cout<<evaluate(board)<<std::endl;
    return 0;
}