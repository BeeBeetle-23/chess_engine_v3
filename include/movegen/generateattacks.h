#include "board.h"
#include "move.h"
void generatePawnAttacks(const Board &board, Move *move_list, Colour colour, int *move_count);
void generateKnightAttacks(const Board &board, Move *move_list, Colour colour, int *move_count);
void generateBishopAttacks(const Board &board, Move *move_list, Colour colour, int *move_count);
void generateQueenAttacks(const Board &board, Move *move_list, Colour colour, int *move_count);
void generateRookAttacks(const Board &board, Move *move_list, Colour colour, int *move_count);
void generateKingAttacks(const Board &board, Move *move_list, Colour colour, int *move_count);