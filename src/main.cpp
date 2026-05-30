#include <iostream>
#include <cstdint>
#include <unordered_set>
#include "board/board.h"
#include "movegen/attacks.h"
#include "movegen/generateattacks.h"
#include "bitboard/perft.h"
#include "search/search.h"
#include <bit>
#include <array>
#include <chrono>
#include "uci/uci.h"
#include <cassert>
using u64 = uint64_t;
int main(){
    
    initAttackTables();
    initMagicTables();
    //uci_loop(board);
    Board board; int depth = 5;
    board.Parse_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    auto start = std::chrono::steady_clock::now();
    //perft_divide(board,depth);
    u64 nodes = perft(board, depth);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Function execution time: " << elapsed.count() << " ms\n";
    //return 0;
    //std::cout<<nodes;
    
    return 0;
}