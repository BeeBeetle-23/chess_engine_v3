#include "board\zobrist.h"
#include <random>
#include <cstdint>
double probes = 0;
double hits = 0;
namespace Zobrist {

    uint64_t pieces[12][64];
    uint64_t castling[16];
    uint64_t en_passant[65];
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
        en_passant[64] = 0;

        black_to_move= splitmix64(state);
    }

} // namespace Zobrist

TTEntry* probe(uint64_t hash) {
    probes++;
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