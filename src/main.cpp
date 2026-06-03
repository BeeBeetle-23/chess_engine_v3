#include <iostream>
#include <cstdint>
#include "evaluation\evaluation.h"
#include "board/board.h"
#include "movegen/attacks.h"
#include "movegen/generateattacks.h"
#include "bitboard/perft.h"
#include "search/search.h"
#include <bit>
#include <array>
#include <chrono>
#include "uci/uci.h"
#include "board/zobrist.h"
#include <cassert>
using u64 = uint64_t;
int main(){
    
    initAttackTables();
    initMagicTables();
    Zobrist::init();
    Board board;int depth = 5;
    //uci_loop(board);
    board.Parse_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    perft_divide(board,depth);
    //std::cout << "EP square = " << board.ep_square << "\n";
    return 0;
}