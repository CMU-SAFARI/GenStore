#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include <cstdint>
#include <iostream>
#include <parallel/algorithm>

int main(int argc, char **argv) {
    std::ifstream fin(argv[1]);
    std::string header;
    std::vector<std::pair<uint64_t, uint64_t>> vals;
    uint64_t hash, pos;
    while (fin >> header) {
        if (header == "myhash") {
            fin >> hash >> pos;
            vals.emplace_back(hash, pos);
        }
    }

    __gnu_parallel::sort(vals.begin(), vals.end());
    for (const auto &[h, p] : vals) {
        std::cout << h << "\t" << p << "\n";
    }

    return 0;
}
