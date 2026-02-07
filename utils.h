

#ifndef BR_USG_UTILS_H
#define BR_USG_UTILS_H

#include <fstream>
#include <regex>
#include <fstream>
#include <vector>
#include <filesystem>

/**
 * @brief Parameters for generating a nearly balanced instance.
 *
 * Expected line format: n1 n2 m1 m2 btw c1 c2
 */
struct NearlyBalancedParams{
    explicit NearlyBalancedParams(const std::string& line);
    int n1_;  ///< Size of community 1.
    int n2_;  ///< Size of community 2.
    int m1_;  ///< Number of intra-community edges in community 1.
    int m2_;  ///< Number of intra-community edges in community 2.
    int btw_; ///< Number of inter-community edges.
    int c1_;  ///< Number of cross edges added inside community 1.
    int c2_;  ///< Number of cross edges added inside community 2.
};

/**
 * @brief Read parameters from a file (one value per line).
 *
 * Parsing is determined by the template specialization in utils.cpp.
 * Reading stops on the first empty line.
 */
template <class T>
std::vector<T> read_params(const std::string& filepath);

/// List file names in a directory (non-recursive).
std::vector<std::string> get_dir_files(const std::string& dir_name);

#endif //BR_USG_UTILS_H
