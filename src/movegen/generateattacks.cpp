#include "board/board.h"
#include "move/move.h"
#include "bitboard/masks.h"
#include "movegen/attacks.h"
#include "movegen/generateattacks.h"
#include "bitboard/bitboard.h"
#include <bit>
#include <cassert>
static constexpr int MAX_MOVES = 256;
using u64 = uint64_t;

void generatePawnMoves(const Board &board, Move *move_list, Colour colour, int *move_count) {
    // --- Entry guards ---
    /*assert(move_list  != nullptr        && "move_list is null");
    assert(move_count != nullptr        && "move_count is null");
    assert(*move_count >= 0             && "move_count is negative");
    assert(*move_count < MAX_MOVES      && "move_list already full on entry");
    assert(board.ep_square == NO_SQUARE || 
           board.ep_square < 64         && "ep_square out of range");*/

    // Safe EP — explicitly compare against your sentinel,
    // not < 64, which silently passes on -1 or 255 depending on signedness
    const u64 ep_bitboard = (board.ep_square != NO_SQUARE)
                          ? (1ULL << board.ep_square)
                          : 0ULL;

    const u64 empty_squares = ~board.occupancies[BOTH];

    // Bounds-checked push: every single move write goes through here
    const auto push_move = [&](Move m) {
        /*assert(*move_count < MAX_MOVES && "move_list overflow in generatePawnMoves");*/
        move_list[(*move_count)++] = m;
    };

    if (colour == WHITE) {
        const u64 enemy_pieces = board.occupancies[BLACK];

        // -------------------------------------------------------
        // PUSHES — bulk bitboard (attack table doesn't model these)
        // -------------------------------------------------------
        const u64 single_pushes = (board.pieces[WP] << 8) & empty_squares;
        const u64 double_pushes = ((single_pushes & RANK_3) << 8) & empty_squares;

        u64 normal_pushes = single_pushes & ~RANK_8;
        while (normal_pushes) {
            const int to_sq = __builtin_ctzll(normal_pushes);
            push_move(Move((Square)(to_sq - 8), (Square)to_sq, QUIET));
            normal_pushes &= normal_pushes - 1;
        }

        u64 promo_pushes = single_pushes & RANK_8;
        while (promo_pushes) {
            const int   to_sq   = __builtin_ctzll(promo_pushes);
            const Square from_sq = (Square)(to_sq - 8);
            push_move(Move(from_sq, (Square)to_sq, QUEEN_PROMOTION));
            push_move(Move(from_sq, (Square)to_sq, ROOK_PROMOTION));
            push_move(Move(from_sq, (Square)to_sq, BISHOP_PROMOTION));
            push_move(Move(from_sq, (Square)to_sq, KNIGHT_PROMOTION));
            promo_pushes &= promo_pushes - 1;
        }

        u64 dbl = double_pushes;
        while (dbl) {
            const int to_sq = __builtin_ctzll(dbl);
            push_move(Move((Square)(to_sq - 16), (Square)to_sq, DOUBLE_PAWN_PUSH));
            dbl &= dbl - 1;
        }

        // -------------------------------------------------------
        // CAPTURES + EP — per-pawn via attack table
        // from_sq is known directly; no offset arithmetic needed,
        // and FILE_A/FILE_H wrapping is already baked into the table
        // -------------------------------------------------------
        u64 pawns = board.pieces[WP];
        while (pawns) {
            const int from_sq   = __builtin_ctzll(pawns);
            const u64 attacks   = pawn_attacks[WHITE][from_sq]; // precomputed, wrap-safe

            // Normal + promo captures
            u64 captures = attacks & enemy_pieces;
            while (captures) {
                const int to_sq = __builtin_ctzll(captures);
                if ((1ULL << to_sq) & RANK_8) {
                    push_move(Move((Square)from_sq, (Square)to_sq, QUEEN_PROMO_CAPTURE));
                    push_move(Move((Square)from_sq, (Square)to_sq, ROOK_PROMO_CAPTURE));
                    push_move(Move((Square)from_sq, (Square)to_sq, BISHOP_PROMO_CAPTURE));
                    push_move(Move((Square)from_sq, (Square)to_sq, KNIGHT_PROMO_CAPTURE));
                } else {
                    push_move(Move((Square)from_sq, (Square)to_sq, CAPTURE));
                }
                captures &= captures - 1;
            }

            // En passant
            u64 ep = attacks & ep_bitboard;
            while (ep) {
                const int to_sq = __builtin_ctzll(ep);
                // Sanity: EP target must be on rank 6 for white
                assert(((1ULL << to_sq) & RANK_6) && "white EP target not on rank 6");
                push_move(Move((Square)from_sq, (Square)to_sq, EP_CAPTURE));
                ep &= ep - 1;
            }

            pawns &= pawns - 1;
        }

    } else { // BLACK

        const u64 enemy_pieces = board.occupancies[WHITE];

        // -------------------------------------------------------
        // PUSHES
        // -------------------------------------------------------
        const u64 single_pushes = (board.pieces[BP] >> 8) & empty_squares;
        const u64 double_pushes = ((single_pushes & RANK_6) >> 8) & empty_squares;

        u64 normal_pushes = single_pushes & ~RANK_1;
        while (normal_pushes) {
            const int to_sq = __builtin_ctzll(normal_pushes);
            push_move(Move((Square)(to_sq + 8), (Square)to_sq, QUIET));
            normal_pushes &= normal_pushes - 1;
        }

        u64 promo_pushes = single_pushes & RANK_1;
        while (promo_pushes) {
            const int    to_sq   = __builtin_ctzll(promo_pushes);
            const Square from_sq = (Square)(to_sq + 8);
            push_move(Move(from_sq, (Square)to_sq, QUEEN_PROMOTION));
            push_move(Move(from_sq, (Square)to_sq, ROOK_PROMOTION));
            push_move(Move(from_sq, (Square)to_sq, BISHOP_PROMOTION));
            push_move(Move(from_sq, (Square)to_sq, KNIGHT_PROMOTION));
            promo_pushes &= promo_pushes - 1;
        }

        u64 dbl = double_pushes;
        while (dbl) {
            const int to_sq = __builtin_ctzll(dbl);
            push_move(Move((Square)(to_sq + 16), (Square)to_sq, DOUBLE_PAWN_PUSH));
            dbl &= dbl - 1;
        }

        // -------------------------------------------------------
        // CAPTURES + EP
        // -------------------------------------------------------
        u64 pawns = board.pieces[BP];
        while (pawns) {
            const int from_sq = __builtin_ctzll(pawns);
            const u64 attacks  = pawn_attacks[BLACK][from_sq];

            u64 captures = attacks & enemy_pieces;
            while (captures) {
                const int to_sq = __builtin_ctzll(captures);
                if ((1ULL << to_sq) & RANK_1) {
                    push_move(Move((Square)from_sq, (Square)to_sq, QUEEN_PROMO_CAPTURE));
                    push_move(Move((Square)from_sq, (Square)to_sq, ROOK_PROMO_CAPTURE));
                    push_move(Move((Square)from_sq, (Square)to_sq, BISHOP_PROMO_CAPTURE));
                    push_move(Move((Square)from_sq, (Square)to_sq, KNIGHT_PROMO_CAPTURE));
                } else {
                    push_move(Move((Square)from_sq, (Square)to_sq, CAPTURE));
                }
                captures &= captures - 1;
            }

            u64 ep = attacks & ep_bitboard;
            while (ep) {
                const int to_sq = __builtin_ctzll(ep);
                // Sanity: EP target must be on rank 3 for black
                assert(((1ULL << to_sq) & RANK_3) && "black EP target not on rank 3");
                push_move(Move((Square)from_sq, (Square)to_sq, EP_CAPTURE));
                ep &= ep - 1;
            }

            pawns &= pawns - 1;
        }
    }
}

void generateKnightMoves(const Board &board, Move *move_list, Colour colour, int *move_count) {
    assert(move_list  != nullptr   && "move_list is null");
    assert(move_count != nullptr   && "move_count is null");
    assert(*move_count >= 0        && "move_count negative");
    assert(*move_count < MAX_MOVES && "move_list full on entry");

    const auto push_move = [&](Move m) {
        assert(*move_count < MAX_MOVES && "move_list overflow in generateKnightMoves");
        move_list[(*move_count)++] = m;
    };

    const u64 us     = board.occupancies[colour];
    const u64 enemy  = board.occupancies[colour == WHITE ? BLACK : WHITE];
    u64 knights      = board.pieces[colour == WHITE ? WN : BN];

    while (knights) {
        int from_sq = pop_lsb(knights);

        u64 attacks = knight_attacks[from_sq] & ~us;
        while (attacks) {
            const int to_sq = __builtin_ctzll(attacks);
            attacks &= attacks - 1;

            const MoveFlag flag = (1ULL << to_sq) & enemy ? CAPTURE : QUIET;
            push_move(Move((Square)from_sq, (Square)to_sq, flag));
        }
    }
}

void generateKingMoves(const Board &board, Move *move_list, Colour colour, int *move_count) {
    /*assert(move_list  != nullptr   && "move_list is null");
    assert(move_count != nullptr   && "move_count is null");
    assert(*move_count >= 0        && "move_count negative");
    assert(*move_count < MAX_MOVES && "move_list full on entry");*/

    const auto push_move = [&](Move m) {
        //assert(*move_count < MAX_MOVES && "move_list overflow in generateKingMoves");
        move_list[(*move_count)++] = m;
    };

    const u64 us    = board.occupancies[colour];
    const u64 enemy = board.occupancies[colour == WHITE ? BLACK : WHITE];
    u64 king        = board.pieces[colour == WHITE ? WK : BK];

    // Exactly one king must exist
    //assert(__builtin_popcountll(king) == 1 && "king bitboard does not have exactly 1 bit");

    while (king) {
        const int from_sq = __builtin_ctzll(king);
        king &= king - 1;

        u64 attacks = king_attacks[from_sq] & ~us;
        while (attacks) {
            const int to_sq = __builtin_ctzll(attacks);
            attacks &= attacks - 1;

            const MoveFlag flag = (1ULL << to_sq) & enemy ? CAPTURE : QUIET;
            push_move(Move((Square)from_sq, (Square)to_sq, flag));
        }
    }

    // Castling — structure unchanged, just routed through push_move
    if (colour == WHITE) {
        if (board.castling_rights & WK_CASTLE) {
            if (!(board.occupancies[BOTH] & ((1ULL << f1) | (1ULL << g1)))) {
                if (!board.isSquareAttacked(e1, BLACK) &&
                    !board.isSquareAttacked(f1, BLACK) &&
                    !board.isSquareAttacked(g1, BLACK)) {
                    push_move(Move(e1, g1, KING_CASTLE));
                }
            }
        }
        if (board.castling_rights & WQ_CASTLE) {
            if (!(board.occupancies[BOTH] & ((1ULL << b1) | (1ULL << c1) | (1ULL << d1)))) {
                if (!board.isSquareAttacked(e1, BLACK) &&
                    !board.isSquareAttacked(d1, BLACK) &&
                    !board.isSquareAttacked(c1, BLACK)) {
                    push_move(Move(e1, c1, QUEEN_CASTLE));
                }
            }
        }
    } else {
        if (board.castling_rights & BK_CASTLE) {
            if (!(board.occupancies[BOTH] & ((1ULL << f8) | (1ULL << g8)))) {
                if (!board.isSquareAttacked(e8, WHITE) &&
                    !board.isSquareAttacked(f8, WHITE) &&
                    !board.isSquareAttacked(g8, WHITE)) {
                    push_move(Move(e8, g8, KING_CASTLE));
                }
            }
        }
        if (board.castling_rights & BQ_CASTLE) {
            if (!(board.occupancies[BOTH] & ((1ULL << b8) | (1ULL << c8) | (1ULL << d8)))) {
                if (!board.isSquareAttacked(e8, WHITE) &&
                    !board.isSquareAttacked(d8, WHITE) &&
                    !board.isSquareAttacked(c8, WHITE)) {
                    push_move(Move(e8, c8, QUEEN_CASTLE));
                }
            }
        }
    }
}

void generateBishopMoves(const Board &board, Move *move_list, Colour colour, int *move_count) {
    // Identify our bishop piece type
    /*Piece bishop_piece = (colour == WHITE) ? WB : BB;
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
    }*/
    u64 bishops = (colour == WHITE)?board.pieces[WB]:board.pieces[BB];
    u64 us = (colour == WHITE)?board.occupancies[WHITE]:board.occupancies[BLACK];
    u64 them = (colour == BLACK)?board.occupancies[WHITE]:board.occupancies[BLACK];
    while(bishops){
        int from = pop_lsb(bishops);
        u64 attacks = bishopAttacks(board.occupancies[BOTH],(Square)from);
        attacks &= ~us;
        while(attacks){
            int to = pop_lsb(attacks);
            MoveFlag flag = ((1ull << to) & them)?CAPTURE:QUIET;
            move_list[(*move_count)++] = Move((Square)from,(Square)to,flag);
        }
    }
}

void generateRookMoves(const Board &board, Move *move_list, Colour colour, int *move_count) {
    // Identify our rook piece type
    /*Piece rook_piece = (colour == WHITE) ? WR : BR;
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
    }*/
    u64 rooks = (colour == WHITE)?board.pieces[WR]:board.pieces[BR];
    u64 us = (colour == WHITE)?board.occupancies[WHITE]:board.occupancies[BLACK];
    u64 them = (colour == BLACK)?board.occupancies[WHITE]:board.occupancies[BLACK];
    while(rooks){
        int from = pop_lsb(rooks);
        u64 attacks = rookAttacks(board.occupancies[BOTH],(Square)from);
        attacks &= ~us;
        while(attacks){
            int to = pop_lsb(attacks);
            MoveFlag flag = ((1ull << to) & them)?CAPTURE:QUIET;
            move_list[(*move_count)++] = Move((Square)from,(Square)to,flag);
        }
    }
}

void generateQueenMoves(const Board &board, Move *move_list, Colour colour, int *move_count) {
    // Identify our queen piece type
    /*Piece queen_piece = (colour == WHITE) ? WQ : BQ;
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
    }*/
    u64 queens = (colour == WHITE)?board.pieces[WQ]:board.pieces[BQ];
    u64 us = (colour == WHITE)?board.occupancies[WHITE]:board.occupancies[BLACK];
    u64 them = (colour == BLACK)?board.occupancies[WHITE]:board.occupancies[BLACK];
    while(queens){
        int from = pop_lsb(queens);
        u64 attacks = bishopAttacks(board.occupancies[BOTH],(Square)from) | rookAttacks(board.occupancies[BOTH],(Square)from);
        attacks &= ~us;
        while(attacks){
            int to = pop_lsb(attacks);
            MoveFlag flag = ((1ull << to) & them)?CAPTURE:QUIET;
            move_list[(*move_count)++] = Move((Square)from,(Square)to,flag);
        }
    }
}

void generateLegalMoves(Board &board,Move *legal_move_list,Colour colour,int *legal_count)
{
    int movecount = 0;

    Move move_list[MAX_MOVES];

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

void generatePawnCaptures(const Board &board, Move *move_list, Colour colour, int *move_count) {
    u64 pawns = (colour == WHITE)?board.pieces[WP]:board.pieces[BP];
    u64 enemies = (colour == WHITE)?board.occupancies[BLACK]:board.occupancies[BLACK];

    while (pawns) {
        Square from = (Square)__builtin_ctzll(pawns);  // LSB = lowest set square
        pop_lsb(pawns);                                 // clear it

        u64 attacks = maskPawnAttacks(from, colour) & enemies;  // only squares with enemy pieces

        while (attacks) {
            Square to = (Square)__builtin_ctzll(attacks);
            pop_lsb(attacks);

            // Check promotion capture (reaching rank 8 or rank 1)
            if ((colour == WHITE && to >= a8) || (colour == BLACK && to <= h1)) {
                for (MoveFlag promo : {QUEEN_PROMO_CAPTURE, ROOK_PROMO_CAPTURE, 
                    BISHOP_PROMO_CAPTURE, KNIGHT_PROMO_CAPTURE}) {
                    move_list[(*move_count)++] = Move(from, to, promo);
                }
            } else {
                move_list[(*move_count)++] = Move(from, to, CAPTURE);
            }
            if (board.ep_square != NO_SQUARE) {
                u64 ep_attacks = maskPawnAttacks(from, colour) & (1ULL << board.ep_square);
                if (ep_attacks) {
                    move_list[(*move_count)++] = Move(from, board.ep_square, EP_CAPTURE);
                }
            }
        }
    }
}

void generateKnightCaptures(const Board &board, Move *move_list, Colour colour, int *move_count){
    u64 knights = (colour == WHITE)?board.pieces[WN]:board.pieces[BN];
    u64 enemies = (colour == WHITE)?board.occupancies[BLACK]:board.occupancies[BLACK];
    while(knights){
        Square from = (Square)__builtin_ctzll(knights);
        pop_lsb(knights);
        u64 attacks = maskKnightAttacks(from)&attacks;
        while(attacks){
            Square to = (Square)__builtin_ctzll(attacks);
            move_list[(*move_count)++] = Move(from,to,CAPTURE);
        }
    }
}

void generateKingCaptures(const Board &board, Move *move_list, Colour colour, int *move_count){
    u64 king = (colour == WHITE)?board.pieces[WK]:board.pieces[BK];
    u64 enemies = (colour == WHITE)?board.occupancies[BLACK]:board.occupancies[BLACK];
    Square from = (colour == WHITE)?board.king_square[WHITE]:board.king_square[BLACK];
    u64 attacks = maskKnightAttacks(from)&attacks;
    while(attacks){
        Square to = (Square)__builtin_ctzll(attacks);
        move_list[(*move_count)++] = Move(from,to,CAPTURE);
    }
    
}

void generateBishopCaptures(const Board &board, Move *move_list, Colour colour, int *move_count){

}

void generateRookCaptures(const Board &board, Move *move_list, Colour colour, int *move_count){
    
}

void generateQueenCaptures(const Board &board, Move *move_list, Colour colour, int *move_count){

}

void generateCaptures(const Board &board, Move *move_list, Colour colour, int *move_count){
    
}