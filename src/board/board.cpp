#include "board/board.h"
#include "move/move.h"
#include "bitboard/bitboard.h"
#include "movegen/attacks.h"
#include "bitboard/masks.h"
#include "board/zobrist.h"
#include "movegen/generateattacks.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cassert>
#include <string>
using u64 = uint64_t;
bool Board::isSquareAttacked(Square square, Colour attacker) const
{
    //Pawn Attacks
    if (attacker == WHITE){
        if (pieces[WP] & pawn_attacks[BLACK][square])  return true;
    }
    else{
        if (pieces[BP] & pawn_attacks[WHITE][square]) return true;
    }
    //Knight Attacks
    if (knight_attacks[square] & (attacker == WHITE ? pieces[WN] : pieces[BN])) return true;
    
    //King attacks
    if (king_attacks[square] & (attacker == WHITE ? pieces[WK] : pieces[BK]))  return true;
    uint64_t occ = occupancies[WHITE] | occupancies[BLACK];
    assert(occ == occupancies[BOTH]);
    //Bishop Attacks
    u64 bishop_attackers = (attacker == WHITE) ? (pieces[WB] | pieces[WQ]) : (pieces[BB] | pieces[BQ]);
    if(bishopAttacks(occ,square) & bishop_attackers) return true;
    
    //Rook Attacks
    u64 rook_attackers = (attacker == WHITE) ? (pieces[WR] | pieces[WQ]) : (pieces[BR] | pieces[BQ]);
    if(rookAttacks(occ,square) & rook_attackers) return true;
    
    return false;
}
void Board::print_board()
{
    std::cout << "\n";

    for (int rank = 7; rank >= 0; rank--)
    {
        std::cout << rank + 1 << " ";

        for (int file = 0; file < 8; file++)
        {
            int sq = rank * 8 + file;

            std::cout
                << piece_to_char((Square)sq)
                << " ";
        }

        std::cout << "\n";
    }

    std::cout << "  a b c d e f g h\n\n";

    if(side_to_move == WHITE){
        printf("WHITE");
    }
    else{
        printf("BLACK");
    }

    std::cout << "EP: " << ep_square << "\n";

    std::cout << "Castling: "
              << ((castling_rights & WK_CASTLE) ? "K" : "")
              << ((castling_rights & WQ_CASTLE) ? "Q" : "")
              << ((castling_rights & BK_CASTLE) ? "k" : "")
              << ((castling_rights & BQ_CASTLE) ? "q" : "")
              << "\n";

    std::cout << "White King: " << king_square[WHITE] << "\n";
    std::cout << "Black King: " << king_square[BLACK] << "\n";

    std::cout << "\n";
}
uint64_t Board::computeHash() const{
    uint64_t hash = 0;

    // pieces
    for (int piece = 0; piece < 12; piece++)
    {
        uint64_t bb = pieces[piece];

        while (bb)
        {
            int sq = pop_lsb(bb);
            hash ^= Zobrist::pieces[piece][sq];
        }
    }
    hash ^= Zobrist::castling[castling_rights];

    if (ep_square < 64) { // Or ep_square != NO_SQUARE
    hash ^= Zobrist::en_passant[ep_square];
    }

    // side
    if (side_to_move == BLACK)
    hash ^= Zobrist::black_to_move;

    return hash;
}

void Board::dump_hash_debug() {
    uint64_t piece_hash = 0;
    // Calculate what the piece hash *should* be
    for (int i = 0; i < 64; i++) {
        if (piece_on[i] != NO_PIECE)
            piece_hash ^= Zobrist::pieces[piece_on[i]][i];
    }

    uint64_t castle_hash = Zobrist::castling[castling_rights];
    uint64_t side_hash = (side_to_move == BLACK) ? Zobrist::black_to_move : 0;
    uint64_t ep_hash = (ep_square != NO_SQUARE) ? Zobrist::en_passant[ep_square] : 0;
    
    uint64_t total = piece_hash ^ castle_hash ^ side_hash ^ ep_hash;

    std::cout << "--- HASH PROBE ---" << std::endl;
    std::cout << "Computed Total: " << total << std::endl;
    std::cout << "Actual Hash:    " << zobrist_hash << std::endl;
    std::cout << "Piece Diff:     " << (piece_hash ^ zobrist_hash) << " (If this is 0, pieces match)" << std::endl;
}

void Board::verify_board(const std::string& context, Move move)
{
    u64 expected_white = 0;
    u64 expected_black = 0;

    // Rebuild occupancies from piece bitboards
    for (int p = WP; p <= WK; p++)
        expected_white |= pieces[p];

    for (int p = BP; p <= BK; p++)
        expected_black |= pieces[p];

    u64 expected_both = expected_white | expected_black;
    assert(expected_white == occupancies[WHITE]);
    assert(expected_black == occupancies[BLACK]);
    assert(expected_both  == occupancies[BOTH]);
    if(zobrist_hash != computeHash()){
        /*printf("board hash mismatch (currently inside board): ");
        printf("\nBoard Hash: %llu",zobrist_hash);
        printf("\nCompute-hash: %llu",computeHash());
        print_board();*/
        dump_hash_debug();
        
    }
    assert(zobrist_hash == computeHash()&&"hash mismatch in current board position.");
    

    // Verify piece_on[] agrees with piece bitboards
    for (int sq = 0; sq < 64; sq++)
    {
        Piece piece = piece_on[sq];

        if (piece == NO_PIECE)
        {
            for (int p = WP; p <= BK; p++)
            {
                if (getBit(pieces[p], sq))
                {
                    std::cout << "EMPTY SQUARE OCCUPIED IN BITBOARD\n";
                    goto fail;
                }
            }
        }
        else
        {
            if (!getBit(pieces[piece], sq))
            {
                std::cout << "piece_on[] disagrees with bitboards\n";
                goto fail;
            }
        }
    }

    // Verify bitboards agree with piece_on[]
    for (int p = WP; p <= BK; p++)
    {
        u64 bb = pieces[p];

        while (bb)
        {
            Square sq = (Square)pop_lsb(bb);

            if (piece_on[sq] != p)
            {
                std::cout << "Bitboard disagrees with piece_on[]\n";
                goto fail;
            }
        }
    }

    // Verify king locations
    if (piece_on[king_square[WHITE]] != WK)
    {
        std::cout << "White king square incorrect\n";
        goto fail;
    }

    if (piece_on[king_square[BLACK]] != BK)
    {
        std::cout << "Black king square incorrect\n";
        goto fail;
    }
    return;

fail:
    std::cout << "\nBOARD VERIFICATION FAILED\n";
    std::cout << "Context: " << context << "\n";
    std::cout << "Move: " ;move.move_into_algebraic();
    std::cout << "Flag: " << (int)move.flag() << "\n";
    std::cout << "Ply: " << ply << "\n";

    print_board();
    
    printf("Exiting...\n");
    exit(1);
    assert(false);
}
void Board::clearBoard(){
    for(int i = 0; i<12; i++){
        pieces[i] = 0ULL;
    }
    std::fill(std::begin(piece_on), std::end(piece_on), NO_PIECE);
    side_to_move = BOTH;
    castling_rights = WK_CASTLE | WQ_CASTLE | BK_CASTLE | BQ_CASTLE;
    ep_square = NO_SQUARE;
    halfmove_clock = 0;
    fullmove_number = 1;
    ply = 0;
    zobrist_hash = 0;
    king_square[WHITE] = king_square[BLACK] = NO_SQUARE;
    occupancies[WHITE] = occupancies[BLACK] = occupancies[BOTH] = 0;
}
void Board::Parse_FEN(const std::string& fen) {
    clearBoard();
    std::stringstream ss(fen);
    std::string piece_part, turn_part, castle_part, ep_part, half_part, full_part;

    // Split FEN by spaces
    ss >> piece_part >> turn_part >> castle_part >> ep_part >> half_part >> full_part;

    // 1. Parse Pieces (Rank 8 down to 1)
    int rank = 7, file = 0;
    for (char c : piece_part) {
        if (c == '/') { rank--; file = 0; }
        else if (isdigit(c)) { file += (c - '0'); }
        else {
            int sq = rank * 8 + file;
            Piece p = char_to_piece(c);
            add_piece(p, static_cast<Square>(sq));
            if (p == WK) king_square[WHITE] = static_cast<Square>(sq);
            if (p == BK) king_square[BLACK] = static_cast<Square>(sq);
            file++;
        }
    }

    // 2. Turn
    side_to_move = (turn_part == "w") ? WHITE : BLACK;
    if (side_to_move == BLACK)
    zobrist_hash ^= Zobrist::black_to_move;

    // 3. Castling
    castling_rights = 0;
    for (char c : castle_part) {
        if (c == 'K') castling_rights |= WK_CASTLE;
        if (c == 'Q') castling_rights |= WQ_CASTLE;
        if (c == 'k') castling_rights |= BK_CASTLE;
        if (c == 'q') castling_rights |= BQ_CASTLE;
    }
    zobrist_hash ^= Zobrist::castling[castling_rights];

    // 4. En Passant
    if (ep_part != "-") {
        int ep_file = ep_part[0] - 'a';
        int ep_rank = ep_part[1] - '1';
        ep_square = static_cast<Square>(ep_rank * 8 + ep_file);
    } else {
        ep_square = NO_SQUARE;
    }
    
    if (ep_square != NO_SQUARE) {
        zobrist_hash ^= Zobrist::en_passant[ep_square];
    }
    // 5. Clocks
    halfmove_clock = std::stoi(half_part);
    fullmove_number = std::stoi(full_part);
}
Piece Board::char_to_piece(char c) {
    switch(c) {
        case 'P': return WP; case 'N': return WN; case 'B': return WB;
        case 'R': return WR; case 'Q': return WQ; case 'K': return WK;
        case 'p': return BP; case 'n': return BN; case 'b': return BB;
        case 'r': return BR; case 'q': return BQ; case 'k': return BK;
        default: return NO_PIECE;
    }
}
char Board::piece_to_char(Square from) {
    switch(piece_on[from]) {
        case WP: return 'P'; case WN: return 'N'; case WB: return 'B';
        case WR: return 'R'; case WQ: return 'Q'; case WK: return 'K';
        case BP: return 'p'; case BN: return 'n'; case BB: return 'b';
        case BR: return 'r'; case BQ: return 'q'; case BK: return 'k';
        default: return 'X';
    }
}
void Board::remove_piece(Piece piece, Square sq)
{
    assert(piece >= 0 && piece <= 11 && "INVALID PIECE ID!");
    assert(piece_on[sq] == piece);
    pieces[piece] = setBitZero(pieces[piece], sq);
    Colour colour = get_piece_colour(piece);
    occupancies[colour] = setBitZero(occupancies[colour], sq);
    occupancies[BOTH] = setBitZero(occupancies[BOTH], sq);
    piece_on[sq] = NO_PIECE;
    zobrist_hash ^= Zobrist::pieces[piece][sq];
    
}
void Board::add_piece(Piece piece, Square sq)
{
    assert(piece_on[sq] == NO_PIECE);
    pieces[piece] = setBitOne(pieces[piece], sq);
    Colour c = get_piece_colour(piece);
    occupancies[c] = setBitOne(occupancies[c], sq);
    occupancies[BOTH] = setBitOne(occupancies[BOTH], sq);
    piece_on[sq] = piece;
    zobrist_hash ^= Zobrist::pieces[piece][sq];
}
void Board::move_piece(Piece piece, Square from, Square to)
{
    //assert(piece_on[from] == piece);
    remove_piece(piece, from);
    add_piece(piece, to);
    
    if (piece == WK)
        king_square[WHITE] = to;
    else if (piece == BK)
        king_square[BLACK] = to;
}
Piece Board::promotion_piece(Colour side,MoveFlag flag) const
{
    if (side == WHITE)
    {
        switch(flag)
        {
            case KNIGHT_PROMOTION:
            case KNIGHT_PROMO_CAPTURE:
                return WN;

            case BISHOP_PROMOTION:
            case BISHOP_PROMO_CAPTURE:
                return WB;

            case ROOK_PROMOTION:
            case ROOK_PROMO_CAPTURE:
                return WR;

            default:
                return WQ;
        }
    }

    switch(flag)
    {
        case KNIGHT_PROMOTION:
        case KNIGHT_PROMO_CAPTURE:
            return BN;

        case BISHOP_PROMOTION:
        case BISHOP_PROMO_CAPTURE:
            return BB;

        case ROOK_PROMOTION:
        case ROOK_PROMO_CAPTURE:
            return BR;

        default:
            return BQ;
    }
}
void Board::make_move(Move move)
{
    
    //verify_board("MAKE_MOVE",move);
    // Save undo information
    history[ply].move             = move;
    history[ply].castling_rights  = castling_rights;
    history[ply].ep_square        = ep_square;
    history[ply].halfmove_clock   = halfmove_clock;
    history[ply].captured_piece   = NO_PIECE;
    history[ply].zobrist_hash = zobrist_hash;
    // Decode move

    const Square from = move.from();
    const Square to   = move.to();
    const MoveFlag flag = move.flag();
    const Colour us = side_to_move;
    const Colour them = (side_to_move == WHITE)?BLACK:WHITE;
    const Piece moving = piece_on[from];

    
    if (ep_square != NO_SQUARE) {
    zobrist_hash ^= Zobrist::en_passant[ep_square];
}
    ep_square = NO_SQUARE;
    switch(flag)
    {
        // QUIET
        case QUIET:{
            move_piece(moving, from, to);
            break;
        }

        // DOUBLE PAWN PUSH
        case DOUBLE_PAWN_PUSH:{
            move_piece(moving, from, to);
            ep_square = (us == WHITE)? Square(to - 8): Square(to + 8);
            break;
        }

        // NORMAL CAPTURE
        case CAPTURE:{
            Piece captured = piece_on[to];
            history[ply].captured_piece = captured;
            remove_piece(captured, to);
            move_piece(moving, from, to);
            break;
        }

        // EN PASSANT
        case EP_CAPTURE:{
            Square cap_sq = (us == WHITE)? Square(to - 8): Square(to + 8);
            Piece captured = piece_on[cap_sq];
            history[ply].captured_piece = captured;
            remove_piece(captured, cap_sq);
            move_piece(moving, from, to);
            break;
        }

        // KING CASTLE
        case KING_CASTLE:{
            if (us == WHITE){
                move_piece(WK, e1, g1);
                move_piece(WR, h1, f1);
            }
            else{
                move_piece(BK, e8, g8);
                move_piece(BR, h8, f8);
            }

            break;
        }

        // QUEEN CASTLE
        case QUEEN_CASTLE:
        {
            if (us == WHITE){
                move_piece(WK, e1, c1);
                move_piece(WR, a1, d1);
            }
            else{
                move_piece(BK, e8, c8);
                move_piece(BR, a8, d8);
            }

            break;
        }

        // PROMOTIONS

        case KNIGHT_PROMOTION:
        case BISHOP_PROMOTION:
        case ROOK_PROMOTION:
        case QUEEN_PROMOTION:{
            remove_piece(moving, from);

            Piece promo = promotion_piece(us, flag);
            add_piece(promo, to);
            break;
        }

        // PROMOTION CAPTURES
        case KNIGHT_PROMO_CAPTURE:
        case BISHOP_PROMO_CAPTURE:
        case ROOK_PROMO_CAPTURE:
        case QUEEN_PROMO_CAPTURE:{
            Piece captured = piece_on[to];

            history[ply].captured_piece = captured;

            remove_piece(captured, to);
            remove_piece(moving, from);

            Piece promo = promotion_piece(us, flag);
            add_piece(promo, to);
            break;
        }
    }
    zobrist_hash ^= Zobrist::castling[history[ply].castling_rights];

    castling_rights &= castle_rights_mask[from];

    castling_rights &= castle_rights_mask[to];

    zobrist_hash ^= Zobrist::castling[castling_rights];

    //update_occupancies();

    // Update clocks

    if (ep_square != NO_SQUARE) {
        zobrist_hash ^= Zobrist::en_passant[ep_square];
    }
    if (moving == WP || moving == BP ||
        history[ply].captured_piece != NO_PIECE)
    {
        halfmove_clock = 0;
    }
    else
    {
        halfmove_clock++;
    }
    // Change side

    side_to_move =(side_to_move == WHITE)? BLACK: WHITE;
    zobrist_hash ^= Zobrist::black_to_move;
    ply++;
    //verify_board("MAKE_MOVE",move);
}
void Board::undo_move(){
    ply--;

    Undo undo = history[ply];
    Move move = undo.move;

    Square from = move.from();
    Square to   = move.to();

    MoveFlag flag = move.flag();

    side_to_move = (side_to_move == WHITE)? BLACK: WHITE;
    
    Colour colour = side_to_move;
    castling_rights = undo.castling_rights;
    ep_square       = undo.ep_square;
    halfmove_clock  = undo.halfmove_clock;

    switch(flag){
        case QUIET:{
            Piece piece = piece_on[to];
            move_piece(piece,to,from);
            break;
        }
        case DOUBLE_PAWN_PUSH:{
            Piece piece = piece_on[to];
            move_piece(piece,to,from);
            break;
        }
        case CAPTURE:{
            Piece piece = piece_on[to];
            Piece captured = undo.captured_piece;
            move_piece(piece,to,from);
            add_piece(captured,to);break;
        }
        case EP_CAPTURE:{
            Piece piece = piece_on[to];
            Piece captured = undo.captured_piece;
            move_piece(piece,to,from);
            Square offset = (colour == WHITE)?Square(to-8):Square(to+8);
            add_piece(captured,offset);break;
        }
        case KING_CASTLE:{
            if(colour == WHITE){
                move_piece(WK,g1,e1);
                move_piece(WR,f1,h1);
            }
            else{
                move_piece(BK,g8,e8);
                move_piece(BR,f8,h8);
            }
            break;
        }
        case QUEEN_CASTLE:{
            if(colour == WHITE){
                move_piece(WK,c1,e1);
                move_piece(WR,d1,a1);
            }
            else{
                move_piece(BK,c8,e8);
                move_piece(BR,d8,a8);
            }
            break;
        }
        case KNIGHT_PROMOTION:
        case QUEEN_PROMOTION:
        case ROOK_PROMOTION:
        case BISHOP_PROMOTION:{
            Piece promo = promotion_piece(colour,flag);
            remove_piece(promo,to);
            (colour == WHITE)?add_piece(WP,from):add_piece(BP,from);
            break;
        }
        case KNIGHT_PROMO_CAPTURE:
        case BISHOP_PROMO_CAPTURE:
        case ROOK_PROMO_CAPTURE:
        case QUEEN_PROMO_CAPTURE:{
            Piece captured = undo.captured_piece;
            Piece promo = promotion_piece(colour,flag);
            remove_piece(promo,to);
            (colour == WHITE)?add_piece(WP,from):add_piece(BP,from);
            add_piece(captured,to);
            break;
        }
    }
    zobrist_hash = undo.zobrist_hash;
    //verify_board("UNDO MOVE",move);
    //update_occupancies();
}


/*void Board::update_occupancies(){
    occupancies[WHITE] =
        pieces[WP] |
        pieces[WN] |
        pieces[WB] |
        pieces[WR] |
        pieces[WQ] |
        pieces[WK];

    occupancies[BLACK] =
        pieces[BP] |
        pieces[BN] |
        pieces[BB] |
        pieces[BR] |
        pieces[BQ] |
        pieces[BK];

    occupancies[BOTH] =
        occupancies[WHITE] |
        occupancies[BLACK];
}u64 Board::get_bishop_attacks(Square square, u64 blockers) const
{
    u64 attacks = 0ULL;

    int r, f;

    int tr = square / 8;
    int tf = square % 8;

    // NE
    for (r = tr + 1, f = tf + 1;
         r <= 7 && f <= 7;
         r++, f++)
    {
        int sq = r * 8 + f;

        attacks |= (1ULL << sq);

        if (blockers & (1ULL << sq))
            break;
    }

    // NW
    for (r = tr + 1, f = tf - 1;
         r <= 7 && f >= 0;
         r++, f--)
    {
        int sq = r * 8 + f;

        attacks |= (1ULL << sq);

        if (blockers & (1ULL << sq))
            break;
    }

    // SE
    for (r = tr - 1, f = tf + 1;
         r >= 0 && f <= 7;
         r--, f++)
    {
        int sq = r * 8 + f;

        attacks |= (1ULL << sq);

        if (blockers & (1ULL << sq))
            break;
    }

    // SW
    for (r = tr - 1, f = tf - 1;
         r >= 0 && f >= 0;
         r--, f--)
    {
        int sq = r * 8 + f;

        attacks |= (1ULL << sq);

        if (blockers & (1ULL << sq))
            break;
    }

    return attacks;
}
u64 Board::get_rook_attacks( Square square,u64 blockers) const
{
    u64 attacks = 0ULL;

    int r, f;

    int tr = square / 8;
    int tf = square % 8;

    // North
    for (r = tr + 1; r <= 7; r++)
    {
        int sq = r * 8 + tf;

        attacks |= (1ULL << sq);

        if (blockers & (1ULL << sq))
            break;
    }

    // South
    for (r = tr - 1; r >= 0; r--)
    {
        int sq = r * 8 + tf;

        attacks |= (1ULL << sq);

        if (blockers & (1ULL << sq))
            break;
    }

    // East
    for (f = tf + 1; f <= 7; f++)
    {
        int sq = tr * 8 + f;

        attacks |= (1ULL << sq);

        if (blockers & (1ULL << sq))
            break;
    }

    // West
    for (f = tf - 1; f >= 0; f--)
    {
        int sq = tr * 8 + f;

        attacks |= (1ULL << sq);

        if (blockers & (1ULL << sq))
            break;
    }

    return attacks;
}*/