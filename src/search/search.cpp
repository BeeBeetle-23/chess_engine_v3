#include "board/board.h"
#include "evaluation/evaluation.h"
#include "movegen/generateattacks.h"
#include "bitboard/bitboard.h"
#include <algorithm>
constexpr int INF = 1000000;
int negamax(Board &board, int depth, int alpha, int beta)
{
    Move move_list[256];
    int legalCount = 0;

    generateLegalMoves(board, move_list, board.side_to_move, &legalCount);

    // Terminal positions FIRST
    if (legalCount == 0)
    {
        if (is_in_check(board))
        {
            // Side to move is checkmated
            return -MATE_SCORE + board.ply;
        }

        // Stalemate
        return 0;
    }

    // Leaf node evaluation
    if (depth == 0)
    {
        int side = (board.side_to_move == WHITE) ? 1 : -1;
        return side * evaluate(board);
    }

    int best_score = -INF;

    for (int i = 0; i < legalCount; i++)
    {
        board.make_move(move_list[i]);

        int score = -negamax(board, depth - 1, -beta, -alpha);

        board.undo_move();

        if (score > best_score)
        {
            best_score = score;
        }

        if (best_score > alpha)
        {
            alpha = best_score;
        }

        // Beta cutoff
        if (alpha >= beta)
        {
            break;
        }
    }

    return best_score;
}

Move findBestMove(Board &board, int depth)
{
    Move move_list[256];
    int legalCount = 0;

    generateLegalMoves(board, move_list, board.side_to_move, &legalCount);

    if (legalCount == 0)
    {
        return Move();
    }

    Move best_move = move_list[0];

    int best_score = -INF;

    int alpha = -INF;
    int beta  = INF;

    for (int i = 0; i < legalCount; i++)
    {
        board.make_move(move_list[i]);

        int score = -negamax(board, depth - 1, -beta, -alpha);

        board.undo_move();

        if (score > best_score)
        {
            best_score = score;
            best_move = move_list[i];
        }

        if (score > alpha)
        {
            alpha = score;
        }
    }

    return best_move;
}