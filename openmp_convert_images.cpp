#include "conversion.hpp"
#include "ppm_utils.hpp"
#include <filesystem>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>
namespace fs = std::filesystem;
using namespace std;

string path = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images";
vector<string> files;                 // Contains all image filenames
double start_time, end_time, elapsed; // For calculating benchmarks

int main(int argc, char **argv) {
    for (const auto &entry : fs::directory_iterator(path)) {
        string file = entry.path().string();
        if (file.find("ppm", 0) == file.npos)
            files.push_back(file);
    }
    cout << "Total no. of files: " << files.size() << "\n";

    int threads;
    cout << "Enter no. of threads to use for conversion: ";
    cin >> threads;
    threads = max(1, min(8, threads));

    start_time = omp_get_wtime();
    convert_to_ppm(files, threads);
    // convert_from_ppm(files, threads);
    end_time = omp_get_wtime();
    elapsed = end_time - start_time;
    cout << "Time taken = " << elapsed << " seconds.\n";

    return 0;
}