#include "board/board.h"
#include "evaluation/evaluation.h"
#include "movegen/generateattacks.h"
#include "bitboard/bitboard.h"
#include <algorithm>
#include <cassert>
constexpr int INF       = 1000000;

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

int negamax(Board &board, int depth, int alpha, int beta) {
    assert(depth >= 0 && "depth went negative");

    if (depth == 0) {
        int side = (board.side_to_move == WHITE) ? 1 : -1;
        return side * evaluate(board);
    }

    Move move_list[256];
    int  legalCount = 0;

    generateLegalMoves(board, move_list, board.side_to_move, &legalCount);

    if (legalCount == 0) {
        // No legal moves: checkmate or stalemate
        // board.ply must be incremented by make_move and decremented by undo_move
        return is_in_check(board) ? (-MATE_SCORE + board.ply) : 0;
    }

    int best_score = -INF - 1; // one below -INF so even -MATE_SCORE beats it

    for (int i = 0; i < legalCount; i++) {
        board.make_move(move_list[i]);

        // Validate only in debug, and only to catch make_move/undo_move bugs
        // — not every node in production
        assert((validate_board(board), true));

        int score = -negamax(board, depth - 1, -beta, -alpha);

        board.undo_move();

        assert((validate_board(board), true));

        if (score > best_score) best_score = score;
        if (score > alpha)      alpha      = score;
        if (alpha >= beta)      break;       // beta cutoff
    }

    return best_score;
}

Move findBestMove(Board &board, int depth) {
    assert(depth >= 1 && "search depth must be at least 1");

    validate_board(board); // validate once at root, not inside the loop

    ScoredMove move_list;
    int  legalCount = 0;

    generateLegalMoves(board, move_list.move, board.side_to_move, &legalCount);

    if (legalCount == 0) return Move(); // caller must handle null move

    Move best_move  = move_list.move[0];
    int  best_score = -INF - 1; // same fix: must be beatable by -MATE_SCORE
    int  alpha      = -INF - 1;
    int  beta       =  INF;
    scoreMoves(board,move_list,legalCount);
    for (int i = 0; i < legalCount; i++) {
        int best = i;
        //Shallow sorting
        for(int j = i+1; j<legalCount; j++){
            if(move_list.score[j] > move_list.score[best]) best = j;
        }
        std::swap(move_list.move[i],move_list.move[best]);
        std::swap(move_list.score[i],move_list.score[best]);

        board.make_move(move_list.move[i]);
        int score = -negamax(board, depth - 1, -beta, -alpha);
        board.undo_move();

        if (score > best_score) {
            best_score = score;
            best_move  = move_list.move[i];
        }

        // Tighten the window for subsequent root moves
        if (score > alpha) alpha = score;

        // No beta cutoff at root — we need to find the actual best move,
        // not just prove a bound. All root moves must be searched.
    }

    return best_move;
}