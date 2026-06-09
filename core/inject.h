//
// Created by Sergei Kudria on 2026/6/9.
//

#ifndef BR_USG_INJECT_H
#define BR_USG_INJECT_H

#include "algo.h"

struct InjectedEdges{
    InjectedEdges(int n_inj, int n_crit): critical_(n_crit), edge2scan_(n_inj) {};
    std::vector<std::pair<long long, int>> critical_;
    std::vector<long long> edge2scan_;
};

InjectedEdges inject(Graph& gr, int n_crit, int num_edge2scan);

void inject(Graph& gr, InjectedEdges& inj);

#endif //BR_USG_INJECT_H
