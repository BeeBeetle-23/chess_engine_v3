#include "board.h"
#include "move.h"
void generatePawnMoves(const Board &board, Move *move_list, Colour colour, int *move_count);
void generateKnightMoves(const Board &board, Move *move_list, Colour colour, int *move_count);
void generateBishopMoves(const Board &board, Move *move_list, Colour colour, int *move_count);
void generateQueenMoves(const Board &board, Move *move_list, Colour colour, int *move_count);
void generateRookMoves(const Board &board, Move *move_list, Colour colour, int *move_count);
void generateKingMoves(const Board &board, Move *move_list, Colour colour, int *move_count);