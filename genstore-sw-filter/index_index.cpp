#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
#include <cstdint>
#include <limits>

int main(int argc, char **argv) {
    size_t bit_width = std::atol(argv[1]);
    std::ifstream ref_in(argv[2]);
    size_t num_upper_bits = std::atol(argv[3]);
    size_t shift = 64 - num_upper_bits;

    size_t bit_width_b = bit_width / 8;
    std::cerr << "bit width: " << bit_width << "\n";

    uint64_t ref_hash = 0;
    uint32_t ref_pos = 0;
    uint8_t pos_counter = 0;

    size_t split = 0;

    while (!ref_in.eof()) {
        auto cur_pos = ref_in.tellg();
        ref_in.read(reinterpret_cast<char*>(&ref_hash) + 8 - bit_width_b, bit_width_b);
        size_t split_cur = ref_hash >> shift;
        if (split_cur > split) {
            std::cout << cur_pos << "\n";
            split = split_cur;
        }
        ref_in.read(reinterpret_cast<char*>(&pos_counter), 1);
        for (size_t i = 0; i < pos_counter; ++i) {
            ref_in.read(reinterpret_cast<char*>(&ref_pos), sizeof(ref_pos));
        }
    }

    return 0;
}
