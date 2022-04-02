#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
#include <cstdint>
#include <limits>
#include <vector>

int main(int argc, char **argv) {
    size_t bit_width = std::atol(argv[1]);

    uint64_t pos;

    std::vector<std::streampos> ref_in_index{0};
    std::vector<std::streampos> read_in_index{0};

    std::ifstream ref_in_i(std::string(argv[2]) + ".index");
    std::ifstream read_in_i(std::string(argv[3]) + ".index");
    assert(ref_in_i.good());
    assert(read_in_i.good());
    while (ref_in_i >> pos) {
        ref_in_index.push_back(pos);
    }
    ref_in_index.push_back(-1);
    while (read_in_i >> pos) {
        read_in_index.push_back(pos);
    }
    read_in_index.push_back(-1);

    assert(ref_in_index.size() == read_in_index.size());

    size_t bit_width_b = bit_width / 8;
    std::cerr << "bit width: " << bit_width << " num_threads: " << ref_in_index.size() - 1 << "\n";


    size_t global_ref_counter = 0;
    size_t global_read_counter = 0;
    size_t global_match_counter = 0;

     
    #pragma omp parallel for num_threads(ref_in_index.size() - 1)
    for (size_t i = 0; i < ref_in_index.size() - 1; ++i) {
         size_t ref_counter = 0;
        size_t read_counter = 0;
        size_t match_counter = 0;

        uint64_t ref_hash = 0;
        uint64_t read_hash = 0;

        uint32_t ref_pos = 0;
        uint64_t read_pos = 0;
        uint8_t pos_counter = 0;
        std::string seq;
        std::string qual;
  
    std::ifstream ref_in(argv[2]);
    std::ifstream read_in(argv[3]);
    assert(ref_in.good());
    assert(read_in.good());
    ref_in.seekg(ref_in_index[i]);
    read_in.seekg(read_in_index[i]);
    assert(ref_in.good());
    assert(read_in.good());
    assert(!ref_in.eof());
    assert(!read_in.eof());
    while (!ref_in.eof() && ref_in.tellg() != ref_in_index[i + 1]) {
        ref_in.read(reinterpret_cast<char*>(&ref_hash) + 8 - bit_width_b, bit_width_b);
        ++ref_counter;
        ref_in.read(reinterpret_cast<char*>(&pos_counter), 1);
        for (size_t i = 0; i < pos_counter; ++i) {
            ref_in.read(reinterpret_cast<char*>(&ref_pos), sizeof(ref_pos));
        }

        while (read_hash < ref_hash && read_in.tellg() != read_in_index[i + 1] && (read_in >> read_hash >> read_pos >> seq >> qual)) {
            read_hash <<= (64 - bit_width);
            ++read_counter;
        }
        if (read_hash != ref_hash)
            continue;

        while (read_hash == ref_hash) {
            ++match_counter;
            if (!read_in.eof() && read_in.tellg() != read_in_index[i + 1]) {
                read_in >> read_hash >> read_pos >> seq >> qual;
                read_hash <<= (64 - bit_width);
                ++read_counter;
            } else {
                break;
            }
        }
    }

    #pragma omp critical
    {
        global_ref_counter += ref_counter;
        global_read_counter += read_counter;
        global_match_counter += match_counter;
    }
    }

    std::cout << global_ref_counter << " " << global_read_counter << " " << global_match_counter << " " << static_cast<double>(global_match_counter) / global_read_counter << "\n";

    return 0;
}
