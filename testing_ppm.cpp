#include "ppm_utils.hpp"
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
#include <string>
#include <vector>
using namespace std;

string infilepath = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images/1090716.ppm";
string outfilepath = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images/1090716_modified.ppm";
double start_time, end_time, elapsed; // For calculating benchmarks

int main() {
    fstream infile(infilepath, ios::in);
    fstream outfile(outfilepath, ios::out);
    PPMObject ppm;
    mt19937 mt(42);

    if (infile.fail()) {
        cout << "Input file doesn't exist. Exiting.\n";
        remove(outfilepath.c_str());
        return 0;
    }

    infile >> ppm;
    cout << "Completed reading from file.\n";

    start_time = omp_get_wtime();
#pragma omp parallel for num_threads(omp_get_max_threads())
    for (int i = 0; i < ppm.height * ppm.width; i++) {
        ppm.pixels[i].red = mt() % 256;
        ppm.pixels[i].blue = mt() % 256;
    }
    end_time = omp_get_wtime();
    elapsed = end_time - start_time;
    cout << "Time taken = " << elapsed << " seconds.\n";

    outfile << ppm;
    cout << "Completed writing to file.\n";

    infile.close();
    outfile.close();

    return 0;
}