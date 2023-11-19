#include "ppm_utils.hpp"
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
#include <string>
#include <vector>
using namespace std;

// string dirpath = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images/";
string infilepath = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images/mayuri_suoh.ppm";
string rgbfilepath = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images/mayuri_suoh_rgb.ppm";
string hsvfilepath = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images/mayuri_suoh_hsv.ppm";
string gsfilepath = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images/mayuri_suoh_gs.ppm";
double start_time, end_time, elapsed; // For calculating benchmarks

int main() {
    fstream infile(infilepath, ios::in);
    fstream rgboutfile(rgbfilepath, ios::out);
    fstream hsvoutfile(hsvfilepath, ios::out);
    // fstream gsoutfile(gsfilepath, ios::out);
    PPMObject ppm;

    if (infile.fail()) {
        cout << "Input file doesn't exist. Exiting.\n";
        remove(rgbfilepath.c_str());
        remove(hsvfilepath.c_str());
        // remove(gsfilepath.c_str());
        return 0;
    }

    infile >> ppm;
    cout << "Completed reading from file.\n";

    // #pragma omp parallel for num_threads(omp_get_max_threads())
    //     for (int i = 0; i < ppm.height * ppm.width; i++) {
    //         ppm.pixels[i].green = 0;
    //     }
    // PPMObject hsv = RGB_to_HSV(ppm, 8);
    // PPMObject rgb = HSV_to_RGB(hsv, 8);
    RGB_to_HSV(ppm, 8);
    hsvoutfile << ppm;

    HSV_to_RGB(ppm, 8);
    rgboutfile << ppm;

    // RGB_to_Grayscale(ppm, 8);
    // gsoutfile << ppm;

    cout << "Completed writing to file.\n";
    infile.close();
    rgboutfile.close();
    hsvoutfile.close();
    // gsoutfile.close();

    return 0;
}

// start_time = omp_get_wtime();
// end_time = omp_get_wtime();
// elapsed = end_time - start_time;
// cout << "Time taken = " << elapsed << " seconds.\n";