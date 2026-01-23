//
// Created by Sergei Kudria on 2026/1/24.
//

#ifndef BR_USG_GENERATOR_H
#define BR_USG_GENERATOR_H

#include <fstream>
#include <regex>
#include "topology.h"

const std::string data_dir(DATA_DIR);
const std::string cur_dir(CUR_DIR);


std::vector<int> read_sparse_sizes(const std::string& filename){
    std::ifstream f(filename);
    if (!f.is_open()) throw std::logic_error("can't open the sparse sizes file");
    std::vector<int> ans;
    std::string(line);
    while (std::getline(f, line)){
        if (line.empty()) break;
        ans.push_back(std::stoi(line));
    }
    return ans;
}

struct NearlyBalancedParams{
    explicit NearlyBalancedParams(std::string& line){
        std::regex re("\\s+");
        std::vector<int> v;
        for (auto it = std::sregex_token_iterator(line.begin(), line.end(), re, -1); it != std::sregex_token_iterator(); ++it)
            v.push_back(std::stoi(it->str()));
        if (v.size() != 7) throw std::runtime_error("Invalid string for nearly balanced params");
        n1_ = v[0];
        n2_ = v[1];
        m1_ = v[2];
        m2_ = v[3];
        btw_ = v[4];
        c1_ = v[5];
        c2_ = v[6];
    }
    int n1_;
    int n2_;
    int m1_;
    int m2_;
    int btw_;
    int c1_;
    int c2_;
};

std::vector<NearlyBalancedParams> read_nb_params_data(const std::string& filename){
    std::ifstream f(filename);
    if (!f.is_open()) throw std::logic_error("can't open the nearly balanced params data file");
    std::vector<NearlyBalancedParams> ans;
    std::string(line);
    while (std::getline(f, line)){
        if (line.empty()) break;
        ans.emplace_back(line);
    }
    return ans;
}

// number of nodes, file to save, probability of edge to get a positive sign
void genSyntheticSparse(int n, const std::string& file, double p_pos);

// returns frustration edges
std::vector<std::pair<int, int>> genSyntheticNearlyBalanced(NearlyBalancedParams params, const std::string& file);

void save_cross_edges(const std::vector<std::pair<int, int>>& ed, const std::string& filename){
    std::ofstream f(filename);
    if (!f.is_open()) throw std::runtime_error("can't open file to save cross edges");
    for (auto el: ed) f << el.first << '\t' << el.second << std::endl;
    f.close();
}

int main(int argc, char** argv){
    const std::string error_line = "Invalid input. The format is: the argument sp or nb for generation "
                                   "of sparse and nearly balanced synthetic data respectively. To modify"
                                   "the size, please modify sparse_sizes and nealy_balanced_sizes files"
                                   "respectively.";

    if (argc != 2) throw std::runtime_error(error_line);
    std::string arg(argv[1]);
    if (arg != "sp" && arg != "nb") throw std::runtime_error(error_line);

    if (arg == "sp"){
        auto sz = read_sparse_sizes(cur_dir + "/sparse_sizes");
        for (auto it: sz)
            genSyntheticSparse(it, data_dir + "/synthetic_sparse/sparse_" + std::to_string(it) + ".txt", 0.85);
    } else{
        auto pms = read_nb_params_data(cur_dir + "/nearly_balanced_sizes");
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
