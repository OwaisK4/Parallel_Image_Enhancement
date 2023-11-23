#include "ppm_utils.hpp"
#include <filesystem>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>
namespace fs = std::filesystem;
using namespace std;

string path = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images/";
vector<string> files;                 // Contains all image filenames
double start_time, end_time, elapsed; // For calculating benchmarks

int main(int argc, char **argv) {
    for (const auto &entry : fs::directory_iterator(path)) {
        string file = entry.path().string();
        if (file.find("png", 0) != file.npos || file.find("jpg", 0) != file.npos || file.find("jpeg", 0) != file.npos || file.find("ppm", 0) != file.npos)
            files.push_back(file);
    }
    cout << "Total no. of files: " << files.size() << "\n";

    int threads = 8;
    int choice = -1;
    cout << "Image conversion from jpg/png to ppm = 0\n";
    cout << "Image conversion from ppm to jpg = 1\n";
    while (choice < 0 || choice > 1) {
        cout << "Enter choice: ";
        cin >> choice;
    }
    cout << "Enter no. of threads to use for conversion: ";
    cin >> threads;
    threads = max(1, min(8, threads));

    start_time = omp_get_wtime();
    if (choice == 0)
        convert_all_to_ppm(files, threads);
    else if (choice == 1)
        convert_all_from_ppm(files, threads);
    end_time = omp_get_wtime();
    elapsed = end_time - start_time;
    cout << "Time taken = " << elapsed << " seconds.\n";

    return 0;
}