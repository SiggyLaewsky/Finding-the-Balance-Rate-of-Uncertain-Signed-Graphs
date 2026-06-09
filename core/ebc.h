//
// Created by Sergei Kudria on 2026/6/9.
//

#ifndef BR_USG_EBC_H
#define BR_USG_EBC_H

#include "graph.h"

class EBC : public Graph {
public:

    explicit EBC(const std::string& filename): Graph(filename) {};
    std::vector<long long> rank_subset(const std::vector<long long>& edg, int top_k);

protected:
    // Warning: any graph update requires a call of eval_compressed!
    void eval_compressed();
    int count_num_common_neighboors(int u, int v);

    std::vector<int> edge_st_;
    std::vector<int> edge_idx_;
    std::vector<bool> v_mask_;
};

#endif //BR_USG_EBC_H
