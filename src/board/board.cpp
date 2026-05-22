#include "board.h"
#include "move.h"
#include "bitboard.h"
inline void Board::remove_piece(Piece piece, Square sq)
{
    pieces[piece] = setBitZero(pieces[piece], sq);
    piece_on[sq] = NO_PIECE;
}
inline void Board::add_piece(Piece piece, Square sq)
{
    pieces[piece] = setBitOne(pieces[piece], sq);
    piece_on[sq] = piece;
}
inline void Board::move_piece(Piece piece, Square from, Square to)
{
    remove_piece(piece, from);
    add_piece(piece, to);
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
    // Save undo information

    history[ply].move             = move;
    history[ply].castling_rights  = castling_rights;
    history[ply].ep_square        = ep_square;
    history[ply].halfmove_clock   = halfmove_clock;
    history[ply].captured_piece   = NO_PIECE;
    
    // Decode move

    const Square from = move.from();
    const Square to   = move.to();

    const MoveFlag flag = move.flag();

    const Colour us =
        side_to_move;

    const Colour them =
        (side_to_move == WHITE)
        ? BLACK
        : WHITE;

    const Piece moving =
        piece_on[from];

    // Clear EP square by default
    ep_square = NO_SQUARE;

    // Handle move types

    switch(flag)
    {
        // QUIET

        case QUIET:
        {
            move_piece(moving, from, to);
            break;
        }

        // DOUBLE PAWN PUSH

        case DOUBLE_PAWN_PUSH:
        {
            move_piece(moving, from, to);

            ep_square =
                (us == WHITE)
                ? Square(to - 8)
                : Square(to + 8);

            break;
        }

        // NORMAL CAPTURE

        case CAPTURE:
        {
            Piece captured = piece_on[to];

            history[ply].captured_piece =
                captured;

            remove_piece(captured, to);

            move_piece(moving, from, to);

            break;
        }

        // EN PASSANT

        case EP_CAPTURE:
        {
            Square cap_sq =
                (us == WHITE)
                ? Square(to - 8)
                : Square(to + 8);

            Piece captured =
                piece_on[cap_sq];

            history[ply].captured_piece =
                captured;

            remove_piece(captured, cap_sq);

            move_piece(moving, from, to);

            break;
        }

        // KING CASTLE

        case KING_CASTLE:
        {
            if (us == WHITE)
            {
                move_piece(WK, e1, g1);
                move_piece(WR, h1, f1);
            }
            else
            {
                move_piece(BK, e8, g8);
                move_piece(BR, h8, f8);
            }

            break;
        }

        // QUEEN CASTLE

        case QUEEN_CASTLE:
        {
            if (us == WHITE)
            {
                move_piece(WK, e1, c1);
                move_piece(WR, a1, d1);
            }
            else
            {
                move_piece(BK, e8, c8);
                move_piece(BR, a8, d8);
            }

            break;
        }

        // PROMOTIONS

        case KNIGHT_PROMOTION:
        case BISHOP_PROMOTION:
        case ROOK_PROMOTION:
        case QUEEN_PROMOTION:
        {
            remove_piece(moving, from);

            Piece promo =
                promotion_piece(us, flag);

            add_piece(promo, to);

            break;
        }

        // PROMOTION CAPTURES

        case KNIGHT_PROMO_CAPTURE:
        case BISHOP_PROMO_CAPTURE:
        case ROOK_PROMO_CAPTURE:
        case QUEEN_PROMO_CAPTURE:
        {
            Piece captured =
                piece_on[to];

            history[ply].captured_piece =
                captured;

            remove_piece(captured, to);

            remove_piece(moving, from);

            Piece promo =
                promotion_piece(us, flag);

            add_piece(promo, to);

            break;
        }
    }

    // Update castling rights

    castling_rights &=
        castle_rights_mask[from];

    castling_rights &=
        castle_rights_mask[to];

    // Update occupancies

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

    // Update clocks

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

    side_to_move =
        (side_to_move == WHITE)
        ? BLACK
        : WHITE;

    ply++;
}

void Board::undo_move(){
    ply--;
    Move move = history[ply].move;
    Undo undo = history[ply];
    const Square from = move.from();
    const Square to   = move.to();
    const MoveFlag flag = move.flag();
    Piece piece = piece_on[to];
    Colour colour = side_to_move;
    const MoveFlag flag = move.flag();
    side_to_move = (side_to_move == WHITE)?BLACK:WHITE;
    halfmove_clock = history[ply].halfmove_clock;

    switch(flag){
        case QUIET:{
            move_piece(piece,to,from);
            break;}
        case DOUBLE_PAWN_PUSH:{
            move_piece(piece,to,from);
            ep_square = NO_SQUARE;
            break;}
        case CAPTURE:{
            Piece captured = undo.captured_piece;
            move_piece(piece,to,from);
            add_piece(captured,from);break;}
        case EP_CAPTURE:{
            Piece captured = undo.captured_piece;
            move_piece(piece,to,from);
            Square offset = (colour == WHITE)?Square(to-8):Square(to+8);
            add_piece(captured,offset);break;}
        case KING_CASTLE:{
            if(colour == WHITE){
                move_piece(WK,g1,e1);
                move_piece(WR,f1,h1);
            }
            else{
                move_piece(BK,g8,e8);
                move_piece(BR,f8,h8);
            }break;}
        case QUEEN_CASTLE:{
            if(colour == WHITE){
                move_piece(WK,c1,e1);
                move_piece(WR,d1,a1);
            }
            else{
                move_piece(BK,c8,e8);
                move_piece(BR,d8,a8);
            }break;}
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
}