#pragma once
#include "iomanip"
#include "board/board.h"
extern int negamax_nodes;
struct SearchStats {
    int depth = 0;
    uint64_t nodes = 0;
    double time_ms = 0.0;
    uint64_t tt_probes = 0;
    uint64_t tt_hits = 0;

    // Automatically calculates Nodes Per Second
    uint64_t get_nps() const {
        if (time_ms <= 0) return 0;
        return static_cast<uint64_t>(nodes / (time_ms / 1000.0));
    }

    // Automatically calculates TT Hit Rate percentage
    double get_hit_rate() const {
        if (tt_probes == 0) return 0.0;
        return (static_cast<double>(tt_hits) / tt_probes) * 100.0;
    }

    // A nice helper to print everything out in a clean format
    void print_summary() const {
        std::cout << "\n=== SEARCH PERFORMANCE SUMMARY ===\n";
        std::cout << "Depth:        " << depth << "\n";
        std::cout << "Nodes:        " << nodes << "\n";
        std::cout << "Time:         " << time_ms << " ms\n";
        std::cout << "NPS:          " << get_nps() << "\n";
        std::cout << "TT Probes:    " << tt_probes << "\n";
        std::cout << "TT Hits:      " << tt_hits << "\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "TT Hit Rate:  " << get_hit_rate() << "%\n";
        std::cout << "==================================\n";
    }

    // Clear stats between runs
    void reset() {
        depth = 0;
        nodes = 0;
        time_ms = 0.0;
        tt_probes = 0;
        tt_hits = 0;
    }
};
int negamax(Board &board,int depth,int alpha,int beta,int ply);
Move findBestMove(Board &board, int depth);
void validate_board(Board& b);