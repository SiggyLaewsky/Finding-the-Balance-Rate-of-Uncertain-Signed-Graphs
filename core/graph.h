//
// Created by Sergei Kudria on 2026/1/23.
//

#ifndef BR_USG_GRAPH_H
#define BR_USG_GRAPH_H

#include <vector>
#include <random>
#include <string>
#include <chrono>

/**
 * @brief Signed graph with uncertain edge existence.
 *
 * Edges are stored in parallel arrays (head_/tail_/s_/dist_). Only the first
 * m_ entries are considered active. Edges beyond m_ are kept to allow
 * temporary removals without reallocations.
 *
 * The expected edge file format is:
 *   n m
 *   head tail sign prob
 *   ...
 * where sign is 0/1 and prob is the Bernoulli mean for edge existence.
 */
struct Graph{

    /// Construct an empty graph.
    Graph();
    /// Load a graph from a file in the expected edge format.
    explicit Graph(const std::string& filename);
    /// Construct a graph from edge lists and Bernoulli means.
    Graph(int n, int m, std::vector<int>& i, std::vector<int>& j, std::vector<double>& p, std::vector<bool>& s);
    /**
     * @brief Decompose the graph into edge components via DFS preprocessing.
     *
     * Each returned component is reindexed to a compact [0..k) node set.
     * Bridge-only edge batches are discarded by this preprocessing step.
     */
    std::vector<Graph> preprocess();
    /**
     * @brief Deactivate the edge (i, j) by swapping it past the active range.
     *
     * Endpoints must match the stored order (head_, tail_).
     */
    void remove_edge(int i, int j);
    /**
     * @brief Reactivate a previously removed edge (i, j).
     *
     * Endpoints must match the stored order (head_, tail_).
     */
    void return_edge(int i, int j);
    /// Save the graph to a file in the expected edge format.
    void save(const std::string& filename);

    int n_ = 0;  ///< Number of nodes.
    int m_ = 0;  ///< Number of active edges (prefix length of edge arrays).
    std::vector<int> head_;  ///< Edge head indices.
    std::vector<int> tail_;  ///< Edge tail indices.
    std::vector<bool> s_;  ///< Edge signs (true = positive).
    std::vector<std::bernoulli_distribution> dist_;  ///< Edge existence distributions.
};

#endif //BR_USG_GRAPH_H
