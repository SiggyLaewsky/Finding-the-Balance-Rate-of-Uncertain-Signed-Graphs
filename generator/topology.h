//
// Created by Sergei Kudria on 2026/1/24.
//

#ifndef BR_USG_TOPOLOGY_H
#define BR_USG_TOPOLOGY_H

#include <unordered_set>
#include <vector>
#include <random>

struct Topology{
    Topology(int n, int m): n_(n), m_(m), head_(m), tail_(m) {};
    void join(Topology& tp);
    void append_edge(int i, int j);
    int n_;
    int m_;
    std::vector<int> head_;
    std::vector<int> tail_;

    std::unordered_set<long long> used;
    static inline long long encode (int u, int v) {
        if (u > v) std::swap(u, v);
        return (long long)u << 32 | v;
    };
};

Topology generateRandom(int n, int m);

Topology generateSparse(int n);

#endif //BR_USG_TOPOLOGY_H
