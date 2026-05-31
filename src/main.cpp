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
#include <cassert>
using u64 = uint64_t;
int main(){
    
    initAttackTables();
    initMagicTables();
    Board board;int depth = 5;
    //uci_loop(board);
    board.Parse_FEN("r2qr1k1/1bp1bppp/p1np1n2/1p2p3/3PP3/1BP2N1P/PP1B1PP1/RN1QR1K1 b - - 0 1");
    auto start = std::chrono::steady_clock::now();
    findBestMove(board,depth).move_into_algebraic();
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Function execution time: " << elapsed.count() << " ms\n";
    std::cout<<"\nNegamax nodes = "<<negamax_nodes<<"\n"<<"Q nodes = "<<q_nodes;
    return 0;
}