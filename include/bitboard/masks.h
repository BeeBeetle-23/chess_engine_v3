#include <cstdint>


// Files are whole continuous bytes in your architecture
// Ranks (Rows) are whole continuous bytes now
constexpr uint64_t RANK_1 = 0x00000000000000FFULL; // Squares 0-7   (a1-h1)
constexpr uint64_t RANK_2 = 0x000000000000FF00ULL; // Squares 8-15  (a2-h2)
constexpr uint64_t RANK_3 = 0x0000000000FF0000ULL; // Squares 16-23 (a3-h3)
constexpr uint64_t RANK_4 = 0x00000000FF000000ULL; // Squares 24-31 (a4-h4)
constexpr uint64_t RANK_5 = 0x000000FF00000000ULL; // Squares 32-39 (a5-h5)
constexpr uint64_t RANK_6 = 0x0000FF0000000000ULL; // Squares 40-47 (a6-h6)
constexpr uint64_t RANK_7 = 0x00FF000000000000ULL; // Squares 48-55 (a7-h7)
constexpr uint64_t RANK_8 = 0xFF00000000000000ULL; // Squares 56-63 (a8-h8)

// Files (Columns) are periodic bits across all bytes
constexpr uint64_t FILE_A = 0x0101010101010101ULL; // Bit 0 of every byte (a1, a2, a3...)
constexpr uint64_t FILE_B = 0x0202020202020202ULL; // Bit 1 of every byte (b1, b2, b3...)
constexpr uint64_t FILE_C = 0x0404040404040404ULL; // Bit 2 of every byte (c1, c2, c3...)
constexpr uint64_t FILE_D = 0x0808080808080808ULL; // Bit 3 of every byte (d1, d2, d3...)
constexpr uint64_t FILE_E = 0x1010101010101010ULL; // Bit 4 of every byte (e1, e2, e3...)
constexpr uint64_t FILE_F = 0x2020202020202020ULL; // Bit 5 of every byte (f1, f2, f3...)
constexpr uint64_t FILE_G = 0x4040404040404040ULL; // Bit 6 of every byte (g1, g2, g3...)
constexpr uint64_t FILE_H = 0x8080808080808080ULL; // Bit 7 of every byte (h1, h2, h3...)