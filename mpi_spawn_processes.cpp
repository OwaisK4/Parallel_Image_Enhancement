#include <filesystem>
#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <string>
#include <vector>
namespace fs = std::filesystem;

std::string path = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images";
std::vector<std::string> files;

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) { // Master process
        for (const auto &entry : fs::directory_iterator(path)) {
            std::string file = entry.path().string();
            if (file.find("ppm", 0) == file.npos)
                files.push_back(file);
        }
        for (auto &it : files) {
            std::cout << it << "\n";
        }
        std::cout << "Total no. of files: " << files.size() << "\n";

        if (size == 1) {
            for (auto &file : files) {
                int index = file.find(".", 0);
                std::string basename = file.substr(0, index);
                std::string command = "convert '" + file + "' -compress none '" + basename + ".ppm'";
                system(command.c_str());
            }
        }
        //  else {
        //     int slaves = size - 1;
        //     int per_slave = files.size() / slaves;
        //     for (int slave = 1; slave <= slaves; slave++) {
        //     }

        //     int remaining = files.size() - (slaves * per_slave);
        // }
    }

    MPI_Finalize();

    return 0;
}