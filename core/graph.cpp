//
// Created by Sergei Kudria on 2026/1/23.
//

#include <fstream>
#include "graph.h"

Graph::Graph(): head_(0), tail_(0), s_(0), dist_(0) {}

Graph::Graph(int n, int m, std::vector<int>& i, std::vector<int>& j, std::vector<double>& p, std::vector<bool>& s):
    n_(n), m_(m), head_(std::move(i)), tail_(std::move(j)), s_(std::move(s)), dist_(m){
    for (auto eid = 0; eid < m_; ++eid) dist_[eid] = std::bernoulli_distribution(p[eid]);
}

Graph::Graph(const std::string &filename) {
    std::ifstream f(filename);
    if (!f.is_open()) throw std::runtime_error("File " + filename + " can't be open");

    f >> n_ >> m_;
    head_.resize(m_);
    tail_.resize(m_);
    dist_.resize(m_);
    s_.resize(m_);

    for (int eid = 0; eid < m_; ++eid){
        int i, j;
        double p;
        bool s;
        f >> i >> j >> s >> p;
        head_[eid] = i;
        tail_[eid] = j;
        s_[eid] = s;
        dist_[eid] = std::bernoulli_distribution(p);
    }
}

std::vector<Graph> Graph::preprocess() {

    std::vector<int> edge_st(n_ + 1, 0);
    std::vector<int> edge_id(2 * m_);

    for (int e = 0; e < m_; ++e) {
        ++edge_st[head_[e]];
        ++edge_st[tail_[e]];
    }
    for (int i = 1; i <= n_; ++i)
        edge_st[i] += edge_st[i - 1];

    for (int e = 0; e < m_; ++e) {
        edge_id[--edge_st[head_[e]]] = e;
        edge_id[--edge_st[tail_[e]]] = e;
    }

    std::vector<bool> is_bridge(m_, false);
    std::vector<bool> is_cutpoint(n_, false);
    std::vector<int> tin(n_, -1), low(n_, -1), parent(n_, -1), next_edge_idx = edge_st;
    int timer = 0;

    std::vector<int> stack_dfs;
    for (int i = 0; i < n_; ++i) {
        if (tin[i] == -1) {
            int root_children = 0;
            stack_dfs.push_back(i);
            tin[i] = low[i] = timer++;

            while (!stack_dfs.empty()) {

                int u = stack_dfs.back();

                if (next_edge_idx[u] < edge_st[u + 1]) {
                    int eid = edge_id[next_edge_idx[u]++];
                    int v = (head_[eid] == u) ? tail_[eid] : head_[eid];

                    if (v == parent[u]) continue;

                    if (tin[v] != -1) {
                        low[u] = std::min(low[u], tin[v]);
                    } else {
                        parent[v] = u;
                        if (u == i) root_children++;

                        tin[v] = low[v] = timer++;
                        stack_dfs.push_back(v);
                    }
                } else {
                    stack_dfs.pop_back();
                    if (!stack_dfs.empty()) {
                        int p = stack_dfs.back();
                        low[p] = std::min(low[p], low[u]);

                        if (low[u] > tin[p]) {
                            for (int k = edge_st[p]; k < edge_st[p + 1]; ++k) {
                                int eid = edge_id[k];
                                if ((head_[eid] == p && tail_[eid] == u) ||
                                    (tail_[eid] == p && head_[eid] == u)) {
                                    is_bridge[eid] = true;
                                    break;
                                }
                            }
                        }

                        if (p != i && low[u] >= tin[p]) is_cutpoint[p] = true;
                    }
                }
            }
            if (root_children > 1) is_cutpoint[i] = true;
        }
    }

    struct Frame {
        int v;
        int it;
    };

    std::vector<Graph> result(0);
    std::vector<bool> visited(n_, false);
    std::vector<bool> visited_edge(m_, false);
    std::vector<int> vmap(n_, -1);
    for (int start = 0; start < n_; ++start) {
        if (visited[start] || is_cutpoint[start]) continue;
        visited[start] = true;
        std::vector<Frame> stack;
        stack.push_back({start, edge_st[start]});

        std::vector<int> component;

        while (!stack.empty()) {
            Frame &f = stack.back();
            int v = f.v;

            if (f.it < edge_st[v + 1]) {
                int eid = edge_id[f.it++];
                int to = (head_[eid] == v ? tail_[eid] : head_[eid]);
                if (is_bridge[eid]) continue;
                if (visited_edge[eid]) continue;

                component.push_back(eid);
                visited_edge[eid] = true;

                if (is_cutpoint[to]) continue;
                if (!visited[to]){
                    stack.push_back({to, edge_st[to]});
                    visited[to] = true;
                }
            }
            else{
                stack.pop_back();
            }
        }

        int new_n = 0;

        auto map_v = [&](int v) {
            int& id = vmap[v];
            if (id == -1) id = new_n++;
            return id;
        };

        std::vector<int> nh, nt;
        std::vector<double> np;
        std::vector<bool> ns;
        for (int ei: component) {
            if (is_bridge[ei]) continue;

            int u = map_v(head_[ei]);
            int v = map_v(tail_[ei]);

            nh.push_back(u);
            nt.push_back(v);
            np.push_back(dist_[ei].p());
            ns.push_back(s_[ei]);
        }

        if (!nh.empty()) {
            result.emplace_back(
                    new_n,
                    static_cast<int>(nh.size()),
                    nh, nt, np, ns
            );
        }

        for (int ei: component){
            vmap[head_[ei]] = -1;
            vmap[tail_[ei]] = -1;
        }
    }

    return result;
}

void Graph::remove_edge(int i, int j) {
    int id = -1;
    for (int eid = 0; eid < m_; ++eid){
        if ((i == head_[eid] && j == tail_[eid])
            || (i == head_[eid] && j == tail_[eid])){
            id = eid;
            break;
        }
    }
    if (id < 0) throw std::runtime_error("Edge " + std::to_string(i) + "-" + std::to_string(j) + " not found");
    if (id == m_ - 1){
        m_--;
        return;
    }

    std::swap(head_[id], head_[m_ - 1]);
    std::swap(tail_[id], tail_[m_ - 1]);
    std::swap(dist_[id], dist_[m_ - 1]);
    std::swap(s_[id], s_[m_ - 1]);
    m_--;
}

void Graph::return_edge(int i, int j) {
    int id = -1;
    for (int eid = m_; eid < (int) head_.size(); ++eid){
        if ((i == head_[eid] && j == tail_[eid])
            || (i == head_[eid] && j == tail_[eid])){
            id = eid;
            break;
        }
    }
    if (id < 0) throw std::runtime_error("Edge " + std::to_string(i) + "-" + std::to_string(j) + " not found");
    if (id == m_){
        m_++;
        return;
    }
    std::swap(head_[id], head_[m_]);
    std::swap(tail_[id], tail_[m_]);
    std::swap(dist_[id], dist_[m_]);
    std::swap(s_[id], s_[m_]);
    m_++;
}

void Graph::save(const std::string &filename) {
    std::ofstream f(filename);
    if (!f.is_open()) throw std::runtime_error("Can't open file " + filename + " to save the graph");
    f << n_ << '\t' << m_ << std::endl;
    for (int eid = 0; eid < m_; ++eid)
        f << head_[eid] << '\t' << tail_[eid] << '\t' << s_[eid] << '\t' << dist_[eid].p() << std::endl;
    f.close();
}
