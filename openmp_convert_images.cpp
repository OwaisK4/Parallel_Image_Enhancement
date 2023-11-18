#include "conversion.hpp"
#include "ppm_utils.hpp"
#include <filesystem>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>
namespace fs = std::filesystem;

std::string path = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images";
std::vector<std::string> files; // Contains all image filenames
double start, end, elapsed;     // For calculating benchmarks

int main(int argc, char **argv) {
    for (const auto &entry : fs::directory_iterator(path)) {
        std::string file = entry.path().string();
        if (file.find("ppm", 0) == file.npos)
            files.push_back(file);
    }
    std::cout << "Total no. of files: " << files.size() << "\n";

    int threads;
    std::cout << "Enter no. of threads to use for conversion: ";
    std::cin >> threads;
    threads = std::min(8, threads);
    threads = std::max(1, threads);

    start = omp_get_wtime();
    convert_to_ppm(files, threads);
    end = omp_get_wtime();
    elapsed = end - start;
    std::cout << "Time taken = " << elapsed << " seconds.\n";

    return 0;
}