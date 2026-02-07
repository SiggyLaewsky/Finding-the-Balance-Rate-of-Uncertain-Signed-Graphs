

#include <fstream>
#include "graph.h"
#include <map>

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
    // Tarjan-style DFS to extract edge components and reindex them as subgraphs.
    struct EdgeInfo {
        int to;
        int original_idx;
    };

    struct Frame {
        int u;
        int p_edge_id;
        int adj_idx;
    };

    std::vector<Graph> result_components;
    if (n_ == 0) return result_components;

    std::vector<int> offset(n_ + 1, 0);

    for (int i = 0; i < m_; ++i) {
        if (head_[i] < n_) offset[head_[i] + 1]++;
        if (tail_[i] < n_ && head_[i] != tail_[i]) offset[tail_[i] + 1]++;
    }

    for (int i = 0; i < n_; ++i) offset[i + 1] += offset[i];

    std::vector<int> current_pos = offset;
    std::vector<EdgeInfo> adj(offset.back());

    // Build adjacency in a compact edge list.
    for (int i = 0; i < m_; ++i) {
        int u = head_[i];
        int v = tail_[i];
        if (u >= n_ || v >= n_ || u == v) continue;

        adj[current_pos[u]++] = {v, i};
        adj[current_pos[v]++] = {u, i};
    }

    std::vector<int> disc(n_, -1);
    std::vector<int> low(n_, -1);
    std::vector<int> edge_stack;
    edge_stack.reserve(m_);

    std::vector<Frame> stack;
    stack.reserve(n_);

    std::vector<int> node_map(n_, -1);
    std::vector<int> used_nodes;
    used_nodes.reserve(n_);

    int timer = 0;

    for (int root = 0; root < n_; ++root) {
        if (disc[root] != -1) continue;

        disc[root] = low[root] = ++timer;
        stack.push_back({root, -1, offset[root]});

        while (!stack.empty()) {
            auto& [u, p_edge, idx] = stack.back();

            if (idx < offset[u + 1]) {
                auto [v, id] = adj[idx];

                if (id == p_edge) {
                    idx++;
                    continue;
                }

                if (disc[v] != -1) {
                    low[u] = std::min(low[u], disc[v]);
                    if (disc[v] < disc[u]) {
                        edge_stack.push_back(id);
                    }
                    idx++;
                } else {
                    edge_stack.push_back(id);
                    disc[v] = low[v] = ++timer;
                    stack.push_back({v, id, offset[v]});
                }
            } else {
                int child = u;
                int child_low = low[child];
                int edge_to_child = p_edge;
                stack.pop_back();

                if (!stack.empty()) {
                    auto& parent_frame = stack.back();
                    int parent = parent_frame.u;
                    low[parent] = std::min(low[parent], child_low);

                    if (child_low >= disc[parent]) {
                        std::vector<int> comp_edges;

                        while (true) {
                            int e = edge_stack.back();
                            edge_stack.pop_back();
                            comp_edges.push_back(e);
                            if (e == edge_to_child) break;
                        }

                        if (child_low == disc[parent]) {
                            // Build a component by reindexing the nodes it touches.
                            Graph comp;
                            used_nodes.clear();
                            int new_idx_counter = 0;

                            comp.head_.reserve(comp_edges.size());
                            comp.tail_.reserve(comp_edges.size());
                            comp.s_.reserve(comp_edges.size());
                            comp.dist_.reserve(comp_edges.size());

                            for (int e_idx : comp_edges) {
                                int h = head_[e_idx];
                                int t = tail_[e_idx];

                                if (node_map[h] == -1) {
                                    node_map[h] = new_idx_counter++;
                                    used_nodes.push_back(h);
                                }
                                if (node_map[t] == -1) {
                                    node_map[t] = new_idx_counter++;
                                    used_nodes.push_back(t);
                                }

                                comp.head_.push_back(node_map[h]);
                                comp.tail_.push_back(node_map[t]);
                                comp.s_.push_back(s_[e_idx]);
                                comp.dist_.push_back(dist_[e_idx]);
                            }

                            comp.n_ = new_idx_counter;
                            comp.m_ = (int)comp.head_.size();
                            result_components.push_back(std::move(comp));

                            for (int node : used_nodes) node_map[node] = -1;
                        }
                    }

                    parent_frame.adj_idx++;
                }
            }
        }
    }

    return result_components;
}

void Graph::remove_edge(int i, int j) {
    int id = -1;
    for (int eid = 0; eid < m_; ++eid){
        if ((i == head_[eid] && j == tail_[eid])
            || (j == head_[eid] && i == tail_[eid])){
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
            || (j == head_[eid] && i == tail_[eid])){
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

std::vector<double> Graph::set_p_logarithmic() {
    std::random_device rd;
    std::mt19937 gen(rd());

    double density = ((double ) m_) / ((double) n_);
    double rng = std::sqrt((double) m_) * std::sqrt(density);

    std::vector<double> p_list(m_);
    for (auto eid = 0; eid < m_; ++eid){
        std::uniform_real_distribution pmean_dist(0.1 / rng, 20.0 / rng);
        dist_[eid] = std::bernoulli_distribution(std::min(1.0, pmean_dist(gen)));
        p_list[eid] = dist_[eid].p();
    }
    return p_list;
}
