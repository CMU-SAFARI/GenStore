#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdint>
#include <vector>
#include <algorithm>

int main(int argc, char **argv) {
    size_t target_bit_width = std::atol(argv[1]);
    std::ifstream fin(argv[2]);

    uint64_t hash;
    uint64_t pos;
    size_t in_bit_width = 64;
    size_t shift = in_bit_width - target_bit_width;
    size_t target_size = (target_bit_width + 7) / 8;
    uint64_t last_hash = 0;
    std::vector<uint64_t> poss;

    auto flush = [&]() {
        if (poss.size()) {
            std::cout.write(reinterpret_cast<char*>(&hash), target_size);
            poss.resize(std::min(poss.size(), (size_t)255));
            uint8_t out_size = poss.size();
            std::cout.write(reinterpret_cast<char*>(&out_size), 1);
            for (uint32_t p : poss) {
                std::cout.write(reinterpret_cast<char*>(&p), sizeof(p));
            }

            poss.clear();
        }
    };

    while (fin >> hash >> pos) {
        hash >>= shift;
        if (hash != last_hash) {
            flush();
            last_hash = hash;
        }
        poss.push_back(pos);
    }

    flush();

    return 0;
}
