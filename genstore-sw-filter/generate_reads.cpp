#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <mutex>

int main(int argc, char **argv) {
    size_t read_length = std::atol(argv[1]);
    std::ifstream fin(argv[3]);

    uint64_t hash;
    uint64_t pos;
    std::string seq;
    std::string qual;
    size_t in_bit_width = 64;
    size_t target_bit_width = std::atol(argv[2]);
    size_t shift = in_bit_width - target_bit_width;
    size_t target_size = (target_bit_width + 7) / 8;

    while (fin >> hash >> pos) {
        hash >>= shift;
        std::string seq(read_length, 'A');
        std::cout << hash << "\t" << pos << "\t" << seq << "\t" << seq << "\n";
    }

    return 0;
}
