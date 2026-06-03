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
    Board board;int depth = 6;
    //uci_loop(board);
    board.Parse_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    /*board.Parse_FEN("r2r2k1/pbpn1ppp/1pn1p3/3pP3/1q1P4/2NB1N2/PP4PP/R2Q1RK1 w - - 0 4");
    auto start = std::chrono::steady_clock::now();
    findBestMove(board,depth).move_into_algebraic();
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Function execution time: " << elapsed.count() << " ms\n";
    std::cout<<"\nNegamax nodes = "<<negamax_nodes<<"\n"<<"Q nodes = "<<q_nodes;*/
    std::cout<<perft(board,depth)<<std::endl;
    return 0;
}