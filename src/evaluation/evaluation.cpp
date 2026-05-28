#include "evaluation/evaluation.h"
#include "board/board.h"
#include <bit>
#include "bitboard/bitboard.h"
int get_base_value(int p){
    return piece_value[p];
}
constexpr int MVV_LVA[6][6] = {
// attacker: P  N  B  R  Q  K
    {15,14,13,12,11,10}, // victim pawn
    {25,24,23,22,21,20}, // victim knight
    {35,34,33,32,31,30}, // victim bishop
    {45,44,43,42,41,40}, // victim rook
    {55,54,53,52,51,50}, // victim queen
    {0,0,0,0,0,0}
};

int getMVVLVA(const Board& board, Move move) {
    // Get attacker type (0-5 regardless of colour)
    int attacker = board.piece_on[move.from()] % 6;
    int victim   = board.piece_on[move.to()]   % 6;
    return MVV_LVA[victim][attacker];
}
int get_positional_value(int p,int square){
    switch(p){
        case 0:{return pawn_square_table[square];}
        case 1:{return knight_square_table[square];}
        case 2:{return bishop_square_table[square];}
        case 3:{return rook_square_table[square];}
        case 4:{return queen_square_table[square];}
        case 5:{return king_middlegame_square_table[square];}
        
        case 6:{return pawn_square_table[square^56];}
        case 7:{return knight_square_table[square^56];}
        case 8:{return bishop_square_table[square^56];}
        case 9:{return rook_square_table[square^56];}
        case 10:{return queen_square_table[square^56];}
        case 11:{return king_middlegame_square_table[square^56];}
        default: return 0;
    }
    return 0;
}
int evaluate(const Board &board){
    int score = 0;
    for(int p = 0; p<12; p++){
        u64 piece = board.pieces[p];
        int piece_val = get_base_value(p);
        while(piece){
            int sq = pop_lsb(piece);
            int positional_val = get_positional_value(p,sq);
            if(p<=5) score += (piece_val+positional_val);
            else score -= (piece_val+positional_val);
        }
    }
    return score;
}
void scoreMoves(const Board &board, ScoredMove moves,int count,Move pvmove){
    for(int i = 0; i<count; i++){
        if(moves.move[i] == pvmove) {
            moves.score[i] = 10'000'000;
            continue;
        }
        const MoveFlag flag = moves.move[i].flag();
        switch(flag){
            case QUEEN_PROMO_CAPTURE: moves.score[i] = 9'000'000;break;
            case ROOK_PROMO_CAPTURE: moves.score[i] = 8'000'000;break;
            case BISHOP_PROMO_CAPTURE:
            case KNIGHT_PROMO_CAPTURE:
            case CAPTURE:{
            moves.score[i] = 7'000'000 + getMVVLVA(board, moves.move[i]);break;}
            case EP_CAPTURE: moves.score[i] = 7'000'000;break;
            default: moves.score[i] = 0;break;
        }
    }
} 
