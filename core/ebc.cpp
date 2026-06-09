//
// Created by Sergei Kudria on 2026/6/9.
//

#include "ebc.h"
#include <unordered_set>
#include <algorithm>

std::vector<long long> EBC::rank_subset(const std::vector<long long>& S_edge_codes, int top_k) {
    auto encode_edge = [](int u, int v) {
        if (u > v) std::swap(u, v);
        return (long long) u << 32 | v;
    };

    std::unordered_map<long long, int> code_to_eid;
    code_to_eid.reserve(m_);
    for (int eid = 0; eid < m_; ++eid) {
        code_to_eid[encode_edge(head_[eid], tail_[eid])] = eid;
    }

    eval_compressed();

    std::unordered_set<int> sources_set;
    std::unordered_set<int> S_eid_s;
    S_eid_s.reserve(S_edge_codes.size());

    for (long long code : S_edge_codes) {
        auto it = code_to_eid.find(code);
        if (it == code_to_eid.end()) continue;
        int eid = it->second;

        sources_set.insert(head_[eid]);
        sources_set.insert(tail_[eid]);
        S_eid_s.insert(eid);
    }

    std::vector<int> sources(sources_set.begin(), sources_set.end());
    std::vector<double> BC(m_, 0.0);

    std::vector<int> dist(n_), sigma(n_);
    std::vector<double> delta(n_);
    std::vector<std::vector<int>> pred_v(n_), pred_e(n_);

    for (int s : sources) {
        std::fill(dist.begin(), dist.end(), -1);
        std::fill(sigma.begin(), sigma.end(), 0);
        std::fill(delta.begin(), delta.end(), 0.0);

        for (int i = 0; i < n_; ++i) {
            pred_v[i].clear();
            pred_e[i].clear();
        }

        std::queue<int> q;
        std::vector<int> order;

        dist[s] = 0;
        sigma[s] = 1;
        q.push(s);

        while (!q.empty()) {
            auto v = q.front();
            q.pop();
            order.push_back(v);

            for (auto pos = edge_st_[v]; pos < edge_st_[v + 1]; ++pos) {
                int eid = edge_idx_[pos];
                auto w = (head_[eid] == v ? tail_[eid] : head_[eid]);
                if (dist[w] < 0) {
                    dist[w] = dist[v] + 1;
                    q.push(w);
                }

                if (dist[w] == dist[v] + 1) {
                    sigma[w] += sigma[v];
                    pred_v[w].push_back(v);
                    pred_e[w].push_back(eid);
                }
            }
        }

        std::reverse(order.begin(), order.end());

        for (int w : order) {
            for (size_t i = 0; i < pred_v[w].size(); ++i) {
                int v   = pred_v[w][i];
                int eid = pred_e[w][i];

                double contrib = (double) sigma[v] / sigma[w] * (1.0 + delta[w]);

                if (S_eid_s.count(eid)) BC[eid] += contrib;
                delta[v] += contrib;
            }
        }
    }

    std::vector<std::pair<long long, double>> res;
    res.reserve(S_edge_codes.size());

    for (long long code : S_edge_codes) {
        auto it = code_to_eid.find(code);
        if (it == code_to_eid.end()) continue;
        int eid = it->second;

        double score;
        if (s_[eid] == 1) {
            score = BC[eid];
        } else {
            auto cn = count_num_common_neighboors(head_[eid], tail_[eid]);
            score = (double) cn / (BC[eid] + 1.0);
        }

        res.emplace_back(code, score);
    }

    std::sort(res.begin(), res.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    // Handle safety limits if requested top_k is larger than valid returns
    int actual_top_k = std::min(top_k, static_cast<int>(res.size()));
    std::vector<long long> ans(actual_top_k);
    for (int i = 0; i < actual_top_k; ++i) {
        ans[i] = res[i].first;
    }

    return ans;
}

void EBC::eval_compressed() {
    edge_st_.assign(n_ + 1, 0);
    edge_idx_.resize(2 * m_);

    for (auto eid = 0; eid < m_; ++eid){
        auto u = head_[eid], v = tail_[eid];
        ++edge_st_[u]; ++edge_st_[v];
    }

    auto sum = 0;
    for (auto i = 0; i < n_; ++i){
        auto tmp = edge_st_[i];
        edge_st_[i] = sum;
        sum += tmp;
    }
    edge_st_[n_] = sum;

    for (auto eid = 0; eid < m_; ++eid){
        auto u = head_[eid], v = tail_[eid];
        edge_idx_[edge_st_[u]++] = eid;
        edge_idx_[edge_st_[v]++] = eid;
    }

    for (auto i = n_; i > 0; --i) edge_st_[i] = edge_st_[i - 1];
    edge_st_[0] = 0;

    v_mask_.assign(n_, true);
}

int EBC::count_num_common_neighboors(int u, int v) {
    int ans = 0;

    for (auto pos = edge_st_[u]; pos < edge_st_[u + 1]; ++pos){
        auto eid = edge_idx_[pos];
        auto w = (head_[eid] == u ? tail_[eid] : head_[eid]);
        v_mask_[w] = false;
    }

    for (auto pos = edge_st_[v]; pos < edge_st_[v + 1]; ++pos){
        auto eid = edge_idx_[pos];
        auto w = (head_[eid] == v ? tail_[eid] : head_[eid]);
        if (!v_mask_[w]) ++ans;
    }

    // recover mask
    for (auto pos = edge_st_[u]; pos < edge_st_[u + 1]; ++pos){
        auto eid = edge_idx_[pos];
        auto w = (head_[eid] == u ? tail_[eid] : head_[eid]);
        v_mask_[w] = true;
    }
    return ans;
}
