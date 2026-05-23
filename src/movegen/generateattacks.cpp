#include "board/board.h"
#include "move/move.h"
#include "bitboard/masks.h"
#include "movegen/attacks.h"
#include "movegen/generateattacks.h"
#include "bitboard/bitboard.h"
#include <bit>
using u64 = uint64_t;

void generatePawnMoves(const Board &board, Move *move_list, Colour colour, int *move_count) {
    u64 empty_squares = ~board.occupancies[BOTH];

    // Build the en passant target bitboard
    // (Assumes an invalid/unset EP square has a value >= 64, like NO_SQUARE)
    u64 ep_bitboard = (board.ep_square < 64) ? (1ULL << board.ep_square) : 0ULL;

    if (colour == WHITE) {
        u64 enemy_pieces = board.occupancies[BLACK];
        
        u64 single_pushes = (board.pieces[WP] << 8) & empty_squares;
        u64 double_pushes = ((single_pushes & RANK_3) << 8) & empty_squares;
        u64 left_captures  = (board.pieces[WP] & ~FILE_A) << 7 & enemy_pieces;
        u64 right_captures = (board.pieces[WP] & ~FILE_H) << 9 & enemy_pieces;

        // Parallel En Passant bitboard calculations
        u64 left_ep  = (board.pieces[WP] & ~FILE_A) << 7 & ep_bitboard;
        u64 right_ep = (board.pieces[WP] & ~FILE_H) << 9 & ep_bitboard;

        // 1. Single Pushes
        u64 normal_pushes = single_pushes & ~RANK_8;
        while (normal_pushes) {
            int to_sq = __builtin_ctzll(normal_pushes);
            int from_sq = to_sq - 8;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, QUIET);
            normal_pushes &= normal_pushes - 1;
        }

        u64 promo_pushes = single_pushes & RANK_8;
        while (promo_pushes) {
            int to_sq = __builtin_ctzll(promo_pushes);
            int from_sq = to_sq - 8;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, QUEEN_PROMOTION);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, ROOK_PROMOTION);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, BISHOP_PROMOTION);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, KNIGHT_PROMOTION);
            promo_pushes &= promo_pushes - 1;
        }

        // 2. Double Pushes
        while (double_pushes) {
            int to_sq = __builtin_ctzll(double_pushes);
            int from_sq = to_sq - 16;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, DOUBLE_PAWN_PUSH);
            double_pushes &= double_pushes - 1;
        }

        // 3. Left Captures
        u64 normal_left = left_captures & ~RANK_8;
        while (normal_left) {
            int to_sq = __builtin_ctzll(normal_left);
            int from_sq = to_sq - 7;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, CAPTURE);
            normal_left &= normal_left - 1;
        }

        u64 promo_left = left_captures & RANK_8;
        while (promo_left) {
            int to_sq = __builtin_ctzll(promo_left);
            int from_sq = to_sq - 7;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, QUEEN_PROMO_CAPTURE);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, ROOK_PROMO_CAPTURE);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, BISHOP_PROMO_CAPTURE);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, KNIGHT_PROMO_CAPTURE);
            promo_left &= promo_left - 1;
        }

        // 4. Right Captures
        u64 normal_right = right_captures & ~RANK_8;
        while (normal_right) {
            int to_sq = __builtin_ctzll(normal_right);
            int from_sq = to_sq - 9;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, CAPTURE);
            normal_right &= normal_right - 1;
        }

        u64 promo_right = right_captures & RANK_8;
        while (promo_right) {
            int to_sq = __builtin_ctzll(promo_right);
            int from_sq = to_sq - 9;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, QUEEN_PROMO_CAPTURE);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, ROOK_PROMO_CAPTURE);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, BISHOP_PROMO_CAPTURE);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, KNIGHT_PROMO_CAPTURE);
            promo_right &= promo_right - 1;
        }

        // 5. En Passant Captures
        while (left_ep) {
            int to_sq = __builtin_ctzll(left_ep);
            int from_sq = to_sq - 7;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, EP_CAPTURE);
            left_ep &= left_ep - 1;
        }
        while (right_ep) {
            int to_sq = __builtin_ctzll(right_ep);
            int from_sq = to_sq - 9;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, EP_CAPTURE);
            right_ep &= right_ep - 1;
        }
    } 
    else { // BLACK
        u64 enemy_pieces = board.occupancies[WHITE];
        
        u64 single_pushes = (board.pieces[BP] >> 8) & empty_squares;
        u64 double_pushes = ((single_pushes & RANK_6) >> 8) & empty_squares;
        u64 left_captures  = (board.pieces[BP] & ~FILE_A) >> 9 & enemy_pieces;
        u64 right_captures = (board.pieces[BP] & ~FILE_H) >> 7 & enemy_pieces;

        // Parallel En Passant bitboard calculations
        u64 left_ep  = (board.pieces[BP] & ~FILE_A) >> 9 & ep_bitboard;
        u64 right_ep = (board.pieces[BP] & ~FILE_H) >> 7 & ep_bitboard;

        // 1. Single Pushes
        u64 normal_pushes = single_pushes & ~RANK_1;
        while (normal_pushes) {
            int to_sq = __builtin_ctzll(normal_pushes);
            int from_sq = to_sq + 8;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, QUIET);
            normal_pushes &= normal_pushes - 1;
        }

        u64 promo_pushes = single_pushes & RANK_1;
        while (promo_pushes) {
            int to_sq = __builtin_ctzll(promo_pushes);
            int from_sq = to_sq + 8;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, QUEEN_PROMOTION);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, ROOK_PROMOTION);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, BISHOP_PROMOTION);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, KNIGHT_PROMOTION);
            promo_pushes &= promo_pushes - 1;
        }

        // 2. Double Pushes
        while (double_pushes) {
            int to_sq = __builtin_ctzll(double_pushes);
            int from_sq = to_sq + 16;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, DOUBLE_PAWN_PUSH);
            double_pushes &= double_pushes - 1;
        }

        // 3. Left Captures
        u64 normal_left = left_captures & ~RANK_1;
        while (normal_left) {
            int to_sq = __builtin_ctzll(normal_left);
            int from_sq = to_sq + 9;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, CAPTURE);
            normal_left &= normal_left - 1;
        }

        u64 promo_left = left_captures & RANK_1;
        while (promo_left) {
            int to_sq = __builtin_ctzll(promo_left);
            int from_sq = to_sq + 9;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, QUEEN_PROMO_CAPTURE);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, ROOK_PROMO_CAPTURE);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, BISHOP_PROMO_CAPTURE);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, KNIGHT_PROMO_CAPTURE);
            promo_left &= promo_left - 1;
        }

        // 4. Right Captures
        u64 normal_right = right_captures & ~RANK_1;
        while (normal_right) {
            int to_sq = __builtin_ctzll(normal_right);
            int from_sq = to_sq + 7;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, CAPTURE);
            normal_right &= normal_right - 1;
        }

        u64 promo_right = right_captures & RANK_1;
        while (promo_right) {
            int to_sq = __builtin_ctzll(promo_right);
            int from_sq = to_sq + 7;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, QUEEN_PROMO_CAPTURE);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, ROOK_PROMO_CAPTURE);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, BISHOP_PROMO_CAPTURE);
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, KNIGHT_PROMO_CAPTURE);
            promo_right &= promo_right - 1;
        }

        // 5. En Passant Captures
        while (left_ep) {
            int to_sq = __builtin_ctzll(left_ep);
            int from_sq = to_sq + 9;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, EP_CAPTURE);
            left_ep &= left_ep - 1;
        }
        while (right_ep) {
            int to_sq = __builtin_ctzll(right_ep);
            int from_sq = to_sq + 7;
            move_list[(*move_count)++] = Move((Square)from_sq, (Square)to_sq, EP_CAPTURE);
            right_ep &= right_ep - 1;
        }
    }
}

void generateKnightMoves(const Board &board, Move *move_list, Colour colour, int *move_count){
    u64 us = (colour == WHITE)?board.occupancies[WHITE]:board.occupancies[BLACK];
    u64 enemy = (colour == WHITE)?board.occupancies[BLACK]:board.occupancies[WHITE];
    u64 knights = (colour == WHITE)?board.pieces[WN]:board.pieces[BN];
    while(knights){
        int from = pop_lsb(knights);
        u64 attacks = knight_attacks[from];
        while(attacks){
            int to = pop_lsb(attacks);
            if(!((1ULL<<to)&us)){
                MoveFlag flag = ((1ULL << to) & enemy)? CAPTURE: QUIET;

                move_list[(*move_count)++] = Move ((Square)from, (Square)to, flag);
            }

        }
    }

}

void generateKingMoves(const Board &board, Move *move_list, Colour colour, int *move_count){
    u64 us = (colour == WHITE)?board.occupancies[WHITE]:board.occupancies[BLACK];
    u64 enemy = (colour == WHITE)?board.occupancies[BLACK]:board.occupancies[WHITE];
    u64 king = (colour == WHITE)?board.pieces[WK]:board.pieces[BK];
    while(king){
        int from = pop_lsb(king);
        u64 attacks = king_attacks[from];//in attacks.cpp
        while(attacks){
            int to = pop_lsb(attacks);
            if(!((1ULL<<to)&us)){
                MoveFlag flag = ((1ULL << to) & enemy)? CAPTURE: QUIET;

                move_list[(*move_count)++] = Move ((Square)from, (Square)to, flag);
            }

        }
    }
     if (colour == WHITE)
        {

            if (board.castling_rights & WK_CASTLE)
            {
                if (!(board.occupancies[BOTH] &
                    ((1ULL << f1) |
                     (1ULL << g1))))
                {
                    if (!board.isSquareAttacked(e1, BLACK) &&
                        !board.isSquareAttacked(f1, BLACK) &&
                        !board.isSquareAttacked(g1, BLACK))
                    {
                        move_list[(*move_count)++] =
                            Move(
                                e1,
                                g1,
                                KING_CASTLE
                            );
                    }
                }
            }

            if (board.castling_rights & WQ_CASTLE)
            {
                if (!(board.occupancies[BOTH] &
                    ((1ULL << b1) |
                     (1ULL << c1) |
                     (1ULL << d1))))
                {
                    if (!board.isSquareAttacked(e1, BLACK) &&
                        !board.isSquareAttacked(d1, BLACK) &&
                        !board.isSquareAttacked(c1, BLACK))
                    {
                        move_list[(*move_count)++] =
                            Move(
                                e1,
                                c1,
                                QUEEN_CASTLE
                            );
                    }
                }
            }
        }
        else
        {

            if (board.castling_rights & BK_CASTLE)
            {
                if (!(board.occupancies[BOTH] &
                    ((1ULL << f8) |
                     (1ULL << g8))))
                {
                    if (!board.isSquareAttacked(e8, WHITE) &&
                        !board.isSquareAttacked(f8, WHITE) &&
                        !board.isSquareAttacked(g8, WHITE))
                    {
                        move_list[(*move_count)++] =
                            Move(
                                e8,
                                g8,
                                KING_CASTLE
                            );
                    }
                }
            }

            if (board.castling_rights & BQ_CASTLE)
            {
                if (!(board.occupancies[BOTH] &
                    ((1ULL << b8) |
                     (1ULL << c8) |
                     (1ULL << d8))))
                {
                    if (!board.isSquareAttacked(e8, WHITE) &&
                        !board.isSquareAttacked(d8, WHITE) &&
                        !board.isSquareAttacked(c8, WHITE))
                    {
                        move_list[(*move_count)++] =
                            Move(
                                e8,
                                c8,
                                QUEEN_CASTLE
                            );
                    }
                }
            }
        }
    }

void generateBishopMoves(const Board &board, Move *move_list, Colour colour, int *move_count) {
    // Identify our bishop piece type
    Piece bishop_piece = (colour == WHITE) ? WB : BB;
    Bitboard bishops = board.pieces[bishop_piece];

    // Grab friend vs enemy occupancy layers
    Colour opponent_colour = (colour == WHITE) ? BLACK : WHITE;
    Bitboard same_color_occ = board.occupancies[colour];
    Bitboard opp_color_occ = board.occupancies[opponent_colour];

    // Diagonal direction shifts: {delta_file, delta_rank}
    const int directions[4][2] = {
        { 1,  1},  // North-East
        {-1,  1},  // North-West
        { 1, -1},  // South-East
        {-1, -1}   // South-West
    };

    while (bishops) {
        // pop_lsb clears the current bit and returns the index
        int from_idx = pop_lsb(bishops);
        Square from_sq = static_cast<Square>(from_idx);

        // Rank-Major Math: File is lower 3 bits, Rank is upper 3 bits
        int file = from_idx & 7;
        int rank = from_idx >> 3;

        // Radiate outward along the 4 diagonal paths
        for (int d = 0; d < 4; ++d) {
            int df = directions[d][0];
            int dr = directions[d][1];

            int cur_file = file + df;
            int cur_rank = rank + dr;

            // Stay within the boundaries of the 8x8 chessboard
            while (cur_file >= 0 && cur_file < 8 && cur_rank >= 0 && cur_rank < 8) {
                
                // Reconstruct the destination square for your new rank-major schema
                Square to_sq = static_cast<Square>((cur_rank << 3) | cur_file);
                Bitboard to_bit = 1ULL << to_sq;

                // 1. Path is blocked by a teammate -> Ray terminates
                if (same_color_occ & to_bit) {
                    break;
                }

                // 2. Path is blocked by an opponent -> Capture, then ray terminates
                if (opp_color_occ & to_bit) {
                    move_list[(*move_count)++] = Move(from_sq, to_sq, CAPTURE);
                    break;
                }

                // 3. Square is empty -> Record quiet move, continue down the ray
                move_list[(*move_count)++] = Move(from_sq, to_sq, QUIET);

                cur_file += df;
                cur_rank += dr;
            }
        }
    }
}

void generateRookMoves(const Board &board, Move *move_list, Colour colour, int *move_count) {
    // Identify our rook piece type
    Piece rook_piece = (colour == WHITE) ? WR : BR;
    Bitboard rooks = board.pieces[rook_piece];

    // Grab friend vs enemy occupancy layers
    Colour opponent_colour = (colour == WHITE) ? BLACK : WHITE;
    Bitboard same_color_occ = board.occupancies[colour];
    Bitboard opp_color_occ = board.occupancies[opponent_colour];

    // Orthogonal direction shifts: {delta_file, delta_rank}
    const int directions[4][2] = {
        { 0,  1},  // North
        { 0, -1},  // South
        { 1,  0},  // East
        {-1,  0}   // West
    };

    while (rooks) {
        int from_idx = pop_lsb(rooks);
        Square from_sq = static_cast<Square>(from_idx);

        int file = from_idx & 7;
        int rank = from_idx >> 3;

        // Radiate outward along the 4 orthogonal paths
        for (int d = 0; d < 4; ++d) {
            int df = directions[d][0];
            int dr = directions[d][1];

            int cur_file = file + df;
            int cur_rank = rank + dr;

            while (cur_file >= 0 && cur_file < 8 && cur_rank >= 0 && cur_rank < 8) {
                Square to_sq = static_cast<Square>((cur_rank << 3) | cur_file);
                Bitboard to_bit = 1ULL << to_sq;

                if (same_color_occ & to_bit) {
                    break;
                }

                if (opp_color_occ & to_bit) {
                    move_list[(*move_count)++] = Move(from_sq, to_sq, CAPTURE);
                    break;
                }

                move_list[(*move_count)++] = Move(from_sq, to_sq, QUIET);

                cur_file += df;
                cur_rank += dr;
            }
        }
    }
}

void generateQueenMoves(const Board &board, Move *move_list, Colour colour, int *move_count) {
    // Identify our queen piece type
    Piece queen_piece = (colour == WHITE) ? WQ : BQ;
    Bitboard queens = board.pieces[queen_piece];

    // Grab friend vs enemy occupancy layers
    Colour opponent_colour = (colour == WHITE) ? BLACK : WHITE;
    Bitboard same_color_occ = board.occupancies[colour];
    Bitboard opp_color_occ = board.occupancies[opponent_colour];

    // Combine Orthogonal and Diagonal directions: {delta_file, delta_rank}
    const int directions[8][2] = {
        { 0,  1}, { 0, -1}, { 1,  0}, {-1,  0}, // Orthogonal (Rook)
        { 1,  1}, {-1,  1}, { 1, -1}, {-1, -1}  // Diagonal (Bishop)
    };

    while (queens) {
        int from_idx = pop_lsb(queens);
        Square from_sq = static_cast<Square>(from_idx);

        int file = from_idx & 7;
        int rank = from_idx >> 3;

        // Radiate outward along all 8 cardinal and ordinal paths
        for (int d = 0; d < 8; ++d) {
            int df = directions[d][0];
            int dr = directions[d][1];

            int cur_file = file + df;
            int cur_rank = rank + dr;

            while (cur_file >= 0 && cur_file < 8 && cur_rank >= 0 && cur_rank < 8) {
                Square to_sq = static_cast<Square>((cur_rank << 3) | cur_file);
                Bitboard to_bit = 1ULL << to_sq;

                if (same_color_occ & to_bit) {
                    break;
                }

                if (opp_color_occ & to_bit) {
                    move_list[(*move_count)++] = Move(from_sq, to_sq, CAPTURE);
                    break;
                }

                move_list[(*move_count)++] = Move(from_sq, to_sq, QUIET);

                cur_file += df;
                cur_rank += dr;
            }
        }
    }
}

void generateLegalMoves(Board &board,Move *legal_move_list,Colour colour,int *legal_count)
{
    int movecount = 0;

    Move move_list[256];

    Colour attacker =
        (colour == WHITE)
        ? BLACK
        : WHITE;

    generatePawnMoves(board, move_list, colour, &movecount);
    generateKnightMoves(board, move_list, colour, &movecount);
    generateBishopMoves(board, move_list, colour, &movecount);
    generateRookMoves(board, move_list, colour, &movecount);
    generateQueenMoves(board, move_list, colour, &movecount);
    generateKingMoves(board, move_list, colour, &movecount);

    *legal_count = 0;

    for (int i = 0; i < movecount; i++)
    {
        board.make_move(move_list[i]);

        if (!board.isSquareAttacked(board.king_square[colour],attacker))
        {
            legal_move_list[*legal_count] =
                move_list[i];

            (*legal_count)++;
        }

        board.undo_move();
    }
}