#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
#include <cstdint>
#include <limits>

int main(int argc, char **argv) {
    size_t bit_width = std::atol(argv[1]);
    std::ifstream read_in(argv[2]);
    size_t num_upper_bits = std::atol(argv[3]);
    size_t shift = 64 - num_upper_bits;

    size_t bit_width_b = bit_width / 8;
    std::cerr << "bit width: " << bit_width << "\n";

    size_t read_counter = 0;
    uint64_t read_hash = 0;
    uint64_t read_pos = 0;
    std::string seq;
    std::string qual;

    size_t split = 0;

    while (true) {
        auto cur_pos = read_in.tellg();
        if (!(read_in >> read_hash >> read_pos >> seq >> qual))
            break;

        read_hash <<= (64 - bit_width);
        size_t split_cur = read_hash >> shift;
        if (split_cur > split) {
            std::cout << cur_pos << "\n";
            split = split_cur;
        }
    }

    return 0;
}
