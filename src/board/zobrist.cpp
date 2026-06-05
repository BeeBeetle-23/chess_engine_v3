#include "board\zobrist.h"
#include "bitboard\bitboard.h"
#include <random>
#include <cstdint>
double probes = 0;
double hits = 0;
namespace Zobrist {

    uint64_t pieces[12][64];
    uint64_t castling[16];
    uint64_t en_passant[64];
    uint64_t black_to_move;

    static uint64_t splitmix64(uint64_t& state) {
        state += 0x9e3779b97f4a7c15ULL;
        uint64_t z = state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }

    void init(uint64_t seed) {
        uint64_t state = seed;

        for (auto& piece : pieces)
            for (auto& key : piece)
                key = splitmix64(state);

        for (auto& key : castling)
            key = splitmix64(state);

        for (int sq = 0; sq < 64; sq++)
            en_passant[sq] = splitmix64(state);

        black_to_move= splitmix64(state);
    }

} // namespace Zobrist


TTEntry* probe(uint64_t hash) {
    TTEntry* entry =
        &transposition_table[tt_index(hash)];
    if (entry->hash == hash){
        hits++;
        return entry;
    }
    return nullptr;
}

void store(uint64_t hash,int score,int depth,TTflag flag,Move best_move) {
    TTEntry& entry = transposition_table[tt_index(hash)];

    if (entry.hash != hash || depth >= entry.depth) {
        entry.hash = hash;
        entry.score = score;
        entry.depth = depth;
        entry.flag = flag;
        entry.best_move = best_move;
    }
}

/*uint64_t computeHash(const Board &board){
    uint64_t hash = 0;

    // pieces
    for (int piece = 0; piece < 12; piece++)
    {
        uint64_t bb = board.pieces[piece];

        while (bb)
        {
            int sq = pop_lsb(bb);
            hash ^= Zobrist::pieces[piece][sq];
        }
    }
    hash ^= Zobrist::castling[board.castling_rights];

    // ep
    hash ^= Zobrist::en_passant[board.ep_square];

    // side
    if (board.side_to_move == BLACK)
        hash ^= Zobrist::black_to_move;

    return hash;
}*/