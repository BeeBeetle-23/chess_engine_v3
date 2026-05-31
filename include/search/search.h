#pragma once
#include "board/board.h"
extern int negamax_nodes;
int negamax(Board &board,int depth,int alpha,int beta,int ply);
Move findBestMove(Board &board, int depth);
void validate_board(Board& b);