//
// Created by Sergei Kudria on 2026/1/24.
//

#ifndef BR_USG_TOPOLOGY_H
#define BR_USG_TOPOLOGY_H

#include <unordered_set>
#include <vector>
#include <random>

/**
 * @brief Lightweight undirected topology used by synthetic generators.
 */
struct Topology{
    /// Initialize topology with n nodes and m placeholder edges.
    Topology(int n, int m): n_(n), m_(m), head_(m), tail_(m) {};
    /// Join another topology by reindexing its nodes and appending its edges.
    void join(Topology& tp);
    /// Append an undirected edge if endpoints are valid and non-equal.
    void append_edge(int i, int j);
    int n_;  ///< Number of nodes.
    int m_;  ///< Number of edges.
    std::vector<int> head_;  ///< Edge head indices.
    std::vector<int> tail_;  ///< Edge tail indices.

    // Optional set for duplicate checks; populate externally if needed.
    std::unordered_set<long long> used;
    static inline long long encode (int u, int v) {
        if (u > v) std::swap(u, v);
        return (long long)u << 32 | v;
    };
};

/// Generate a connected random topology with n nodes and m edges.
Topology generateRandom(int n, int m);

/// Generate a sparse topology with a two-phase edge growth pattern.
Topology generateSparse(int n);

#endif //BR_USG_TOPOLOGY_H
