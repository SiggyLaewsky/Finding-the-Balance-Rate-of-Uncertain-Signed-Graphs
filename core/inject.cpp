//
// Created by Sergei Kudria on 2026/6/9.
//

#include "inject.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>

InjectedEdges inject(Graph& gr, int n_crit, int num_edge2scan){
    if (num_edge2scan < n_crit) {
        throw std::logic_error("Num2scan must be >= n_crit");
    }

    InjectedEdges ans(num_edge2scan, n_crit);

    std::random_device rd;
    int random_seed = gr.m_ * gr.n_; // Deterministic for reproducibility
    std::mt19937 gen_rand(rd());
    std::mt19937 gen_dtrm(random_seed);
    std::uniform_real_distribution<double> p_dist(0.5, 1.0);
    std::uniform_int_distribution<int> v_dist(0, gr.n_ - 1);

    DSU dsu(gr.n_);
    std::unordered_set<long long> used;

    auto encode = [](int u, int v) {
        if (u > v) std::swap(u, v);
        return ((long long)u << 32) | (unsigned int)v;
    };

    for (auto eid = 0; eid < gr.m_; ++eid){
        used.insert(encode(gr.head_[eid], gr.tail_[eid]));
        auto unite = dsu.unite(gr.head_[eid], gr.tail_[eid], gr.s_[eid]);
        if (!unite) throw std::runtime_error("Instance is not balanced");
        gr.dist_[eid] = std::bernoulli_distribution(p_dist(gen_rand));
    }

    for (int iCrit = 0; iCrit < n_crit; ++iCrit) {
        int u = -1, v = -1;
        int cnt = 0;
        bool sgn = false;
        bool found = false;

        while (cnt++ < 10000) {
            u = v_dist(gen_dtrm);
            v = v_dist(gen_dtrm);
            while (u == v || used.find(encode(u, v)) != used.end()) {
                u = v_dist(gen_dtrm);
                v = v_dist(gen_dtrm);
            }

            auto p1 = dsu.find(u), p2 = dsu.find(v);
            if (p1.second != p2.second) {
                continue;
            }

            sgn = (p1.first ^ p2.first);
            found = true;
            break;
        }

        if (!found) {
            throw std::runtime_error("Instance failed to find a cross-edge");
        }

        long long edge_code = encode(u, v);
        ans.critical_[iCrit] = std::pair<long long, int>(edge_code, sgn);
        used.insert(edge_code);

        std::cout << "Injected: " << std::min(u, v) << "-" << std::max(u, v) << std::endl;

        gr.m_++;
        gr.head_.push_back(u);
        gr.tail_.push_back(v);
        gr.dist_.emplace_back(p_dist(gen_rand));
        gr.s_.push_back(sgn);

        auto unite = dsu.unite(u, v, sgn);
        if (unite) throw std::runtime_error("Unknown error");
    }

    ans.edge2scan_.clear();

    if (gr.m_ <= num_edge2scan) {
        for (int eid = 0; eid < gr.m_; ++eid)
            ans.edge2scan_.push_back(encode(gr.head_[eid], gr.tail_[eid]));
    } else {
        std::vector<int> pool(gr.m_ - n_crit);
        std::iota(pool.begin(), pool.end(), 0);

        std::mt19937 rng(gen_dtrm);
        std::vector<int> sampled_indices;

        std::sample(pool.begin(), pool.end(), std::back_inserter(sampled_indices), num_edge2scan - n_crit, rng);

        for (int eid : sampled_indices) {
            ans.edge2scan_.push_back(encode(gr.head_[eid], gr.tail_[eid]));
        }

        for (int i = 0; i < n_crit; ++i) {
            int eid = gr.m_ - i - 1;
            ans.edge2scan_.push_back(encode(gr.head_[eid], gr.tail_[eid]));
        }
    }

    return ans;
}

void inject(Graph& gr, InjectedEdges& inj){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution p_dist(0.5, 1.0);

    for (auto& ed: inj.critical_){
        int u = static_cast<int>(ed.first >> 32), v = static_cast<int>(ed.first & 0xFFFFFFFFLL);
        int sgn = ed.second;
        gr.m_++;
        gr.head_.push_back(u);
        gr.tail_.push_back(v);
        gr.dist_.emplace_back(p_dist(gen));
        gr.s_.push_back(sgn);
    }
}
