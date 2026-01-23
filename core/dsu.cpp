//
// Created by Sergei Kudria on 2026/1/23.
//

#include <numeric>
#include "dsu.h"

DSU::DSU(int n): parent(n), rank(n, 0), rel(n, false) {
    std::iota(parent.begin(), parent.end(), 0);
}

void DSU::reset() {
    std::iota(parent.begin(), parent.end(), 0);
    rel.assign(rel.size(), false);
    rank.assign(rank.size(), 0);
}

std::pair<bool, int> DSU::find(int u) {
    if (parent[u] == u) return {false, u};
    auto root = find(parent[u]);
    auto rel_u = rel[u] ^ root.first;
    rel[u] = rel_u;
    parent[u] = root.second;
    return {rel_u, root.second};
}

bool DSU::unite(int u, int v, bool sign) {
    int rootU = find(u).second, rootV = find(v).second;
    bool targetRel = !sign;

    if (rootU != rootV) {
        if (rank[rootU] < rank[rootV]) {
            parent[rootU] = rootV;
            rel[rootU] = rel[u] ^ targetRel ^ rel[v];
        } else {
            parent[rootV] = rootU;
            rel[rootV] = rel[v] ^ targetRel ^ rel[u];
            if (rank[rootU] == rank[rootV]) rank[rootU]++;
        }
        return true;
    } else
        return (rel[u] ^ rel[v]) == targetRel;
}

