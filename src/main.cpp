#include <iostream>
#include <cstdint>
#include "move/move.h"
#include "board/board.h"
#include "movegen/attacks.h"
#include "movegen/generateattacks.h"
#include <array>
using u64 = uint64_t;
u64 perft(Board& board, int depth)
{
    if (depth == 0)
        return 1ULL;

    Move move_list[256];
    int move_count = 0;

    generateLegalMoves(board,move_list,board.side_to_move,&move_count);

    u64 nodes = 0ULL;

    for (int i = 0; i < move_count; i++)
    {
        board.make_move(move_list[i]);

        nodes += perft(board, depth - 1);

        board.undo_move();
    }

    return nodes;
}
int main()
{
    initAttackTables();

    Board board;
    board.reset();

    for (int depth = 1; depth <= 5; depth++)
    {
        std::cout
            << "Depth "
            << depth
            << ": "
            << perft(board, depth)
            << '\n';
    }

    return 0;
}