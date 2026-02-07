
#ifndef BR_USG_GENERATOR_H
#define BR_USG_GENERATOR_H

#include "../utils.h"
#include "topology.h"

const std::string data_dir(DATA_DIR);
const std::string cur_dir(CUR_DIR);

/**
 * @brief Generate a sparse synthetic signed graph.
 * @param n Number of nodes.
 * @param file Output path.
 * @param p_pos Probability that an edge is positive.
 */
void genSyntheticSparse(int n, const std::string& file, double p_pos);

/**
 * @brief Generate a nearly balanced synthetic graph.
 * @param params Size and density parameters.
 * @param file Output path.
 * @return List of injected "frustration" edges.
 */
std::vector<std::pair<int, int>> genSyntheticNearlyBalanced(NearlyBalancedParams params, const std::string& file);

/// Save a list of cross edges (u, v) to a tab-separated file.
void save_cross_edges(const std::vector<std::pair<int, int>>& ed, const std::string& filename){
    std::ofstream f(filename);
    if (!f.is_open()) throw std::runtime_error("can't open file to save cross edges");
    for (auto el: ed) f << el.first << '\t' << el.second << std::endl;
    f.close();
}

/**
 * @brief Generator entry point.
 *
 * Usage: BR_GENERATOR sp|nb
 *   sp - generate sparse synthetic graphs (sizes from sparse_sizes)
 *   nb - generate nearly balanced graphs (params from nearly_balanced_sizes)
 */
int main(int argc, char** argv){
    const std::string error_line = "Invalid input. The format is: the argument sp or nb for generation "
                                   "of sparse and nearly balanced synthetic data respectively. To modify"
                                   "the size, please modify sparse_sizes and nealy_balanced_sizes files"
                                   "respectively.";

    if (argc != 2) throw std::runtime_error(error_line);
    std::string arg(argv[1]);
    if (arg != "sp" && arg != "nb") throw std::runtime_error(error_line);

    if (arg == "sp"){
        auto sz = read_params<int>(cur_dir + "/sparse_sizes");
        for (auto it: sz)
            genSyntheticSparse(it, data_dir + "/synthetic_sparse/sparse_" + std::to_string(it) + ".txt", 0.85);
    } else{
        auto pms = read_params<NearlyBalancedParams>(cur_dir + "/nearly_balanced_sizes");
        for (auto it: pms) {
            auto ce = genSyntheticNearlyBalanced(it, data_dir + "/synthetic_nearly_balanced/nb_" +
                                std::to_string(it.n1_) + "_" + std::to_string(it.n2_) + ".txt");
            save_cross_edges(ce, data_dir + "/synthetic_nearly_balanced/nb_" + std::to_string(it.n1_) +
                                 + "_" + std::to_string(it.n2_) + "_cross_edges.txt");
        }
    }

    return 0;
}

#endif //BR_USG_GENERATOR_H
