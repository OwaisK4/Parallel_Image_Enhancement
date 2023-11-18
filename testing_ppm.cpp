#include "ppm_utils.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>
using namespace std;

string filepath = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images/girls_kissing.ppm";

int main() {
    fstream file(filepath, ios::in);
    PPMObject ppm;

    file >> ppm;
    cout << ppm << "\n";

    return 0;
}