#include <filesystem>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>
namespace fs = std::filesystem;

std::string path = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images";
std::vector<std::string> files;
double start, end, elapsed;

int main(int argc, char **argv) {
    for (const auto &entry : fs::directory_iterator(path)) {
        std::string file = entry.path().string();
        if (file.find("ppm", 0) == file.npos)
            files.push_back(file);
    }
    // for (auto &it : files) {
    //     std::cout << it << "\n";
    // }
    std::cout << "Total no. of files: " << files.size() << "\n";

    int threads = 8;
    std::cout << "Enter no. of threads to use for conversion: ";
    std::cin >> threads;
    threads = std::min(8, threads);
    threads = std::max(1, threads);

    start = omp_get_wtime();
#pragma omp parallel for num_threads(threads)
    for (int i = 0; i < files.size(); i++) {
        std::string file = files[i];
        int index = file.find(".", 0);
        std::string basename = file.substr(0, index);
        std::string command = "convert '" + file + "' -compress none '" + basename + ".ppm'";
        system(command.c_str());
    }
    end = omp_get_wtime();
    elapsed = end - start;
    std::cout << "Time taken = " << elapsed << " seconds.\n";

    return 0;
}