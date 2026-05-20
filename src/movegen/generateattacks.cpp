#include "board.h"
#include "move.h"
#include "masks.h"
#include "generateattacks.h"
using u64 = uint64_t;

void generatePawnAttacks(const Board &board, Move *move_list, Colour colour, int *move_count) {
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
