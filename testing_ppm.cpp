#include "ppm_utils.hpp"
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
#include <string>
#include <vector>
using namespace std;

string dirpath = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images/";
string filename = "owais";
double start_time, end_time, elapsed; // For calculating benchmarks

int main() {
    fstream infile(dirpath + filename + ".ppm", ios::in);
    if (infile.fail()) {
        cout << "Input file doesn't exist. Exiting.\n";
        return 0;
    }
    fstream gsoutfile(dirpath + filename + "_grayscale.ppm", ios::out);
    fstream redoutfile(dirpath + filename + "_rgb_red.ppm", ios::out);
    fstream greenoutfile(dirpath + filename + "_rgb_green.ppm", ios::out);
    fstream blueoutfile(dirpath + filename + "_rgb_blue.ppm", ios::out);
    fstream bluroutfile(dirpath + filename + "_blurred.ppm", ios::out);

    PPMObject ppm;
    infile >> ppm;
    cout << "Completed reading from file.\n";

    int threads;
    cout << "Enter no. of threads to use for conversion: ";
    cin >> threads;
    threads = max(1, min(8, threads));
    start_time = omp_get_wtime();

    PPMObject grayscale = RGB_to_Grayscale(ppm, threads);
    PPMObject red_ppm = RGB_Extract_Red(ppm, threads);
    PPMObject green_ppm = RGB_Extract_Green(ppm, threads);
    PPMObject blue_ppm = RGB_Extract_Blue(ppm, threads);
    vector<HSV> hsv = RGB_to_HSV(ppm, threads);
    BlurImage(ppm, hsv, threads);
    HSV_to_RGB(ppm, hsv, threads);

    end_time = omp_get_wtime();
    elapsed = end_time - start_time;
    cout << "Time taken = " << elapsed << " seconds.\n";

#pragma omp parallel sections
    {
#pragma omp section
        {
            gsoutfile << grayscale;
        }
#pragma omp section
        {
            redoutfile << red_ppm;
        }
#pragma omp section
        {
            greenoutfile << green_ppm;
        }
#pragma omp section
        {
            blueoutfile << blue_ppm;
        }
#pragma omp section
        {
            bluroutfile << ppm;
        }
    }
    cout << "Completed writing to file.\n";

    infile.close();
    bluroutfile.close();
    gsoutfile.close();
    redoutfile.close();
    greenoutfile.close();
    blueoutfile.close();

    return 0;
}