#include <iostream>
#include <string>
#include "movegen/generateattacks.h"
#include <sstream>
#include "board/board.h"
#include "search/search.h"
#include "move/move.h"
#include "bitboard/bitboard.h"

void uci_loop(Board& board) {
    // Disable output buffering so commands print instantly to Cutechess
    std::setbuf(stdout, NULL); 
    
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            std::cout << "id name ChessEngineV3\n";
            std::cout << "id author Abhi\n";
            std::cout << "uciok\n";
        }
        else if (line == "isready") {
            std::cout << "readyok\n";
        }
        else if (line == "ucinewgame") {
            //ekfcifwodmjvorru

        }
        else if (line.rfind("position", 0) == 0) {
            std::istringstream stream(line);
            std::string token;
            stream >> token;
            stream >> token;

            if (token == "startpos") {
                board.Parse_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                stream >> token;
            } 
            else if (token == "fen") {
                std::string fen_str = "";
                for (int i = 0; i < 6; i++) {
                    stream >> token;
                    fen_str += token + " ";
                }
                board.Parse_FEN(fen_str);
                stream >> token;
            }
            if (token == "moves") {
                std::string move_str;
                while (stream >> move_str) {
                    Move move = algebraic_into_move(board, move_str);
                    if(move == Move()){/*
                        std::cout << "\n====================================\n";
                        std::cout << "CRITICAL PARSE ERROR on move: " << move_str << "\n";
                        std::cout << "Side to move: " << (board.side_to_move == 0 ? "WHITE" : "BLACK") << "\n";
                        
                        // Reveal what the engine thinks is actually legal
                        ScoredMove list;
                        int count = 0;
                        generateLegalMoves(board, list.move, board.side_to_move, &count);
                        
                        std::cout << "Engine thinks there are only " << count << " legal moves:\n";
                        for (int i = 0; i < count; i++) {
                            list.move[i].move_into_algebraic();
                            std::cout << " ";
                        }
                        std::cout << "\n====================================\n";
                        break; // Stop the loop immediately so we don't corrupt the board*/
                        printf("Empty move");
                    }
                    else{    
                    board.make_move(move);
                    }
                }
            }
        }
        else if (line.rfind("go", 0) == 0) {
            std::istringstream stream(line);
            std::string token;
            int depth = 4;
            while (stream >> token) {
                if (token == "depth") {
                    stream >> depth;
                    break;
                }
            }
            Move best = findBestMove(board, depth);
            std::cout << "bestmove ";
            best.move_into_algebraic();
            std::cout << "\n";
        }
        else if (line == "quit") {
            break;
        }
    }
}