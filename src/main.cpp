#include <iostream>
#include <cstdint>
#include "move/move.h"
#include "board/board.h"
#include "movegen/attacks.h"
#include "movegen/generateattacks.h"
#include <array>
#include <vector>
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
void perft_divide(Board& board, int depth) {
    if (depth == 0) return;
    int legal_count = 0;
    u64 total_nodes = 0; // Kept consistent with u64 from perft
    Move move_list[256];
    
    generateLegalMoves(board, move_list, board.side_to_move, &legal_count);

    std::cout << "\n--- DIVIDE DEPTH " << depth << " ---\n";

    // Loop strictly up to legal_count, not the full 256 size
    for (int i = 0; i < legal_count; i++) {
        board.make_move(move_list[i]);
        
        // Count nodes for the child branch
        u64 move_nodes = perft(board, depth - 1);
        total_nodes += move_nodes;
        
        board.undo_move();

        // Print individual move results
        move_list[i].print_into_algebraic(); 
        std::cout << ": " << move_nodes << "\n";
    }

    std::cout << "\nTotal Nodes: " << total_nodes << "\n\n";
}
int main()
{
    initAttackTables();
    Board board;
    board.Parse_FEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    perft_divide(board,5);
    return 0;
}