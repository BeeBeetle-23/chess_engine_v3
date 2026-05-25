#include <iostream>
#include <cstdint>
#include "move/move.h"
#include "board/board.h"
#include "movegen/attacks.h"
#include "movegen/generateattacks.h"
#include "bitboard/perft.h"
#include "search/search.h"
#include <array>
#include "uci/uci.h"
#include <vector>
using u64 = uint64_t;
int main(){
    initAttackTables();
    Board board;
    uci_loop(board);
    return 0;
}