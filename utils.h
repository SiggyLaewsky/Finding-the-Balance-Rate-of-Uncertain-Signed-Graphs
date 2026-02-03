//
// Created by Sergei Kudria on 2026/2/2.
//

#ifndef BR_USG_UTILS_H
#define BR_USG_UTILS_H

#include <fstream>
#include <regex>
#include <fstream>
#include <vector>
#include <filesystem>

struct NearlyBalancedParams{
    explicit NearlyBalancedParams(const std::string& line);
    int n1_;
    int n2_;
    int m1_;
    int m2_;
    int btw_;
    int c1_;
    int c2_;
};

template <class T>
std::vector<T> read_params(const std::string& filepath);

std::vector<std::string> get_dir_files(const std::string& dir_name);

#endif //BR_USG_UTILS_H
