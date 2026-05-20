#pragma once
#include <cstdint>
#include <move.h>
using Bitboard = uint64_t;
enum Colour : uint8_t{
    WHITE, BLACK, BOTH
};
enum Piece : uint8_t {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK,
    NO_PIECE
};
enum Castling : uint8_t {
    WK_CASTLE = 1,
    WQ_CASTLE = 2,
    BK_CASTLE = 4,
    BQ_CASTLE = 8
};

struct Undo {

    Move move;

    uint8_t castling_rights;

    Square ep_square;

    uint16_t halfmove_clock;

    Piece captured_piece;
};

class Board {

public:

    // Piece bitboards
    Bitboard pieces[12];

    // Occupancies
    Bitboard occupancies[3];

    // Side to move
    Colour side_to_move;

    // Castling rights
    uint8_t castling_rights;

    // En passant square
    Square ep_square;

    // Rule bookkeeping
    uint16_t halfmove_clock;
    uint16_t fullmove_number;

    // Cached king squares
    Square king_square[2];

    // Move history
    Undo history[1024];

    int ply = 0;

public:

    void make_move(Move move);

    void undo_move();

    void update_occupancies();
};