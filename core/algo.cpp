//
// Created by HighlySkilledCoder on 2026/1/23.
//

#include <omp.h>
#include "algo.h"

std::vector<double> naive_MC_sample(Graph& gr, int n_samples, int num_threads){
    std::vector<double> br_ans(n_samples);

    std::vector<std::mt19937> gen_vec(num_threads);
    std::vector<DSU> dsu_vec(num_threads, DSU(gr.n_));
    std::vector<std::vector<std::bernoulli_distribution>> dist_vec(num_threads, gr.dist_);
    for (int thread_id = 0; thread_id < num_threads; ++thread_id) {
        std::random_device rd;
        gen_vec[thread_id] = std::mt19937(rd());
    }

#pragma omp parallel for num_threads(num_threads) schedule(guided, 128)
    for (int smp = 0; smp < n_samples; ++smp){
        auto thread_id = omp_get_thread_num();
        auto& dsu = dsu_vec[thread_id];
        auto& gen = gen_vec[thread_id];
        auto& dist = dist_vec[thread_id];

        dsu.reset();
        bool bal = true;
        for (int eid = 0; eid < gr.m_; ++eid){
            int u = gr.head_[eid], v = gr.tail_[eid];
            if (dist[eid](gen)){
                if (!dsu.unite(u, v, gr.s_[eid])){
                    bal = false;
                    break;
                }
            }
        }
        br_ans[smp] = (bal ? 1.0 : 0.0);
    }

    return br_ans;
}

std::vector<double> rao_blackwell_sample_single(Graph& gr, int n_samples, int num_threads){
    std::vector<double> br_ans(n_samples);
    std::vector<double> q(gr.m_);
    for (int eid = 0; eid < gr.m_; ++eid) q[eid] = 1.0 - gr.dist_[eid].p();

    std::vector<std::mt19937> gen_vec(num_threads);
    std::vector<DSU> dsu_vec(num_threads, DSU(gr.n_));
    std::vector<std::vector<std::bernoulli_distribution>> dist_vec(num_threads, gr.dist_);
    for (int thread_id = 0; thread_id < num_threads; ++thread_id) {
        std::random_device rd;
        gen_vec[thread_id] = std::mt19937(rd());
    }

#pragma omp parallel for num_threads(num_threads) schedule(guided, 128)
    for (int smp = 0; smp < n_samples; ++smp){
        auto thread_id = omp_get_thread_num();
        auto& dsu = dsu_vec[thread_id];
        auto& gen = gen_vec[thread_id];
        auto& dist = dist_vec[thread_id];

        double p = 1.0;
        dsu.reset();
        for (int eid = 0; eid < gr.m_; ++eid){
            int u = gr.head_[eid], v = gr.tail_[eid];
            auto f1 = dsu.find(u), f2 = dsu.find(v);
            if (f1.second != f2.second){
                if (dist[eid](gen)) dsu.unite(u, v, gr.s_[eid]);
            } else{
                if (f1.first ^ f2.first ^ !gr.s_[eid]) p *= q[eid];
            }
        }
        br_ans[smp] = p;
    }

    return br_ans;
}

std::vector<std::vector<double>> rao_blackwell_sample(Graph& gr, int n_samples, int num_threads){
    auto gr_comps = gr.preprocess();
    std::vector<std::vector<double>> br_ans(0);
    for (auto& comp: gr_comps) br_ans.emplace_back(rao_blackwell_sample_single(comp, n_samples, num_threads));
    return br_ans;
}

double eval_rbal(Graph& gr, int n_samples, int num_threads){
    auto res = rao_blackwell_sample(gr, n_samples, num_threads);
    double prod = 1.0;
    for (auto& cmp: res)
        prod *= std::accumulate(cmp.begin(), cmp.end(), 0.0) / ((double) n_samples);
    return prod;
}

std::vector<std::pair<int, int>> find_FES(Graph& gr, int n_samples, double delta, int num_threads){
    std::vector<std::pair<int, int>> ans(0);
    while (gr.m_ > 0) {
        int best_eid_i = -1, best_eid_j = -1;
        while (best_eid_i < 0) {
            double curr = eval_rbal(gr, n_samples, num_threads);
            if (curr > 1 - delta) break;
            for (int eid = 0; eid < gr.m_; ++eid) {
                int i = gr.head_[eid], j = gr.tail_[eid];
                gr.remove_edge(i, j);
                double eval = eval_rbal(gr, n_samples, num_threads);
                if (eval > curr) {
                    best_eid_i = i;
                    best_eid_j = j;
                    curr = eval;
                }
                gr.return_edge(i, j);
            }
        }
        if (best_eid_i == -1) break;
        ans.emplace_back(best_eid_i, best_eid_j);
        gr.remove_edge(best_eid_i, best_eid_j);
    }

    for (auto& ed: ans) gr.return_edge(ed.first, ed.second);

    return ans;
}