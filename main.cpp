

#include "experiments.h"

int main(int argc, char* argv[]){

    std::string error_line = "\nArguments: \n-i\t\tExperiment ID (1, 2, 3).\n-d\t\tDirectory for saving results.\n"
                             "-ns\t\tNumber of samples per evaluation of balance rate.\n-nt\t\tNumber of threads to use."
                             "\nNumber of samples is integer > 1. Number of threads is positive integer.";

    int experiment, n_samples = 100, n_threads = 1;
    std::string dir2save;

    for (int k = 1; k < argc; ++k) {
        std::string arg = argv[k];

        if (arg == "-i" && k + 1 < argc) {
            experiment = std::stoi(argv[++k]);
        }
        else if (arg == "-ns" && k + 1 < argc){
            n_samples = std::stoi(argv[++k]);
        }
        else if (arg == "-nt" && k + 1 < argc){
            n_threads = std::stoi(argv[++k]);
        }
        else if (arg == "-d" && k + 1 < argc) {
            dir2save = std::string(argv[++k]);
        }
        else {
            throw std::runtime_error(error_line);
        }
    }

    if (n_samples < 1 || n_threads < 1) throw std::runtime_error(error_line);

    if (experiment == 1){
        run_efficiency_experiment(dir2save, n_threads, n_samples);
    }
    else if (experiment == 2){
        run_p_eta_experiment(dir2save, n_threads, n_samples);
    }
    else if (experiment == 3){
        run_cross_edge_experiment(n_threads, n_samples);
    } else{
        throw std::runtime_error(error_line);
    }

    return 0;
}