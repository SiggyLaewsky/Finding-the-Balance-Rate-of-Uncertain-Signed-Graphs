//
// Created by Sergei Kudria on 2026/1/23.
//

#ifndef BR_USG_DSU_H
#define BR_USG_DSU_H

#include <vector>

struct DSU {
    explicit DSU(int n);
    void reset();
    std::pair<bool, int> find(int u);
    bool unite(int u, int v, bool sign);
    std::vector<int> parent;
    std::vector<bool> rel;
    std::vector<int> rank;
};

#endif //BR_USG_DSU_H
