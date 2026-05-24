#include "evaluation/evaluation.h"
#include "board/board.h"
#include <bit>
#include "bitboard/bitboard.h"
int get_base_value(int p){
    return piece_value[p];
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