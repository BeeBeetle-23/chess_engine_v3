#include "board/board.h"
#include "evaluation/evaluation.h"
#include "movegen/generateattacks.h"
#include "bitboard/bitboard.h"
#include <algorithm>
#include <cassert>
#include "board/zobrist.h"
#define MAX_PLY 20
constexpr int INF  = 1000000;
Move pv[MAX_PLY][MAX_PLY];
int pv_length[MAX_PLY];
int negamax_nodes = 0;

void validate_board(const Board& b) {
    u64 white = 0ULL;
    for (int p = WP; p <= WK; p++) white |= b.pieces[p];
    assert(white == b.occupancies[WHITE] && "white occupancy mismatch");

    u64 black = 0ULL;
    for (int p = BP; p <= BK; p++) black |= b.pieces[p];
    assert(black == b.occupancies[BLACK] && "black occupancy mismatch");

    assert((white & black) == 0ULL      && "white/black pieces overlap");
    assert((white | black) == b.occupancies[BOTH] && "BOTH occupancy mismatch");

    // King sanity
    assert(__builtin_popcountll(b.pieces[WK]) == 1 && "white must have exactly 1 king");
    assert(__builtin_popcountll(b.pieces[BK]) == 1 && "black must have exactly 1 king");
}

int negamax(Board &board, int depth, int alpha, int beta, int ply) {
    int current_alpha = alpha;
    negamax_nodes++;
    pv_length[ply] = ply; 

    // Check Extension Logic
    bool in_check = is_in_check(board);
    if (in_check) {
        depth++;
    }
    if (depth <= 0) {
        return Quiesce(board, alpha, beta);
    }

    ScoredMove move_list;
    int legalCount = 0;
    generateLegalMoves(board, move_list.move, board.side_to_move, &legalCount);

    if (legalCount == 0)
        return in_check ? (-MATE_SCORE + ply) : 0;

    // Move ordering (TT best move goes here later!)
    scoreMoves(board, move_list, legalCount, pv[0][ply]);

    int best_score = -INF;
    Move best_move_this_node = Move(); // Needed for TT storage later

    for (int i = 0; i < legalCount; i++) {
        // Selection sort
        int best = i;
        for (int j = i + 1; j < legalCount; j++)
            if (move_list.score[j] > move_list.score[best]) best = j;
        std::swap(move_list.move[i],  move_list.move[best]);
        std::swap(move_list.score[i], move_list.score[best]);

        board.make_move(move_list.move[i]);
        // Notice the standard negamax parameters
        int score = -negamax(board, depth - 1, -beta, -alpha, ply + 1);
        board.undo_move();

        if (score >= beta) {
            // Beta cutoff (This is a LOWER_BOUND flag for the TT)
            return score; 
        }

        if (score > best_score) {
            best_score = score;

            if (score > alpha) {
                alpha = score;
                best_move_this_node = move_list.move[i];

                // Fix 2: Secure the PV line update safely
                pv[ply][ply] = move_list.move[i];
                for (int next = ply + 1; next < pv_length[ply + 1]; next++) {
                    pv[ply][next] = pv[ply + 1][next];
                }
                pv_length[ply] = pv_length[ply + 1]; 
            }
        }
    }

    // TT Store happens here right before returning!
    return best_score;
}

Move findBestMove(Board &board, int max_depth) {
    for (int depth = 1; depth <= max_depth; depth++) {
        negamax(board, depth, -INF, INF, 0);
    }
    return pv[0][0];  // pv[0] is now the full best line
}