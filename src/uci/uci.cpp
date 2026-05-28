#include <iostream>
#include <string>
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
                    board.make_move(move);
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
            Move best = findBestMove(board, depth,Move());
            std::cout << "bestmove ";
            best.move_into_algebraic();
            std::cout << "\n";
        }
        else if (line == "quit") {
            break;
        }
    }
}