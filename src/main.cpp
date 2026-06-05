#include <iostream>
#include <cstdint>
#include "board/board.h"
#include "movegen/attacks.h"
#include "movegen/generateattacks.h"
#include "bitboard/perft.h"
#include "search/search.h"
#include <bit>
#include <array>
#include <chrono>
#include "uci/uci.h"
#include <cstdlib>
#include "board/zobrist.h"
#include <cassert>
using u64 = uint64_t;
int main(){
    
    initAttackTables();
    initMagicTables();
    Zobrist::init();
    Board board; int depth = 4;
    uci_loop(board);
    //board.Parse_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");//Startpos
    //board.Parse_FEN("8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1");//K-P endgame
    //board.Parse_FEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");//Kiwipete
    //uci_loop(board);
    //std::cout << "EP square = " << board.ep_square << "\n";
    //std::cout<<perft(board,depth);
    
    //handle_go_command(board,depth);
    return 0;
}