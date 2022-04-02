generate: generate_index.cpp
	g++ -O3 -o generate_index generate_index.cpp -march=core-avx2 -fopenmp -pthread

index_index: index_index.cpp
	g++ -O3 -o index_index index_index.cpp -march=core-avx2 -fopenmp -pthread

index_reads: index_reads.cpp
	g++ -O3 -o index_reads index_reads.cpp -march=core-avx2 -fopenmp -pthread

reads: generate_reads.cpp
	g++ -O3 -o generate_reads generate_reads.cpp -march=core-avx2 -fopenmp -pthread

check: check_files.cpp
	g++ -O3 -o check_files check_files.cpp -march=core-avx2 -fopenmp -pthread

check_mt: check_files_split.cpp
	g++ -O3 -o check_files_mt check_files_split.cpp -march=core-avx2 -fopenmp -pthread -g
