#include <omp.h>
#include <string>
#include <vector>

void convert_to_ppm(std::vector<std::string> files, int threads) {
#pragma omp parallel for num_threads(threads)
    for (int i = 0; i < files.size(); i++) {
        std::string file = files[i];
        int index = file.find(".", 0);
        std::string basename = file.substr(0, index);
        std::string command = "convert '" + file + "' -compress none '" + basename + ".ppm'";
        system(command.c_str());
        remove(file.c_str());
    }
}

void convert_from_ppm(std::vector<std::string> files, int threads) {
#pragma omp parallel for num_threads(threads)
    for (int i = 0; i < files.size(); i++) {
        std::string file = files[i];
        int index = file.find(".", 0);
        std::string basename = file.substr(0, index);
        std::string command = "convert '" + file + "' '" + basename + ".jpg'";
        system(command.c_str());
        remove(file.c_str());
    }
}