
#include "topology.h"

void Topology::join(Topology &tp) {
    head_.resize(m_ + tp.m_);
    tail_.resize(m_ + tp.m_);
    for (auto pos = 0; pos < tp.m_; ++pos){
        head_[m_ + pos] = n_ + tp.head_[pos];
        tail_[m_ + pos] = n_ + tp.tail_[pos];
    }
    n_ += tp.n_;
    m_ += tp.m_;
}

void Topology::append_edge(int i, int j) {
    if (i < 0 || i >= n_ || j < 0 || j >= n_ || i == j) throw std::runtime_error("invalid edge");
    if (used.count(encode(i, j)) > 0) return;
    head_.push_back(i);
    tail_.push_back(j);
    m_++;
}

Topology generateRandom(int n, int m){
    Topology tp(n, 0);
    if (m < n - 1) throw std::runtime_error("m must be >= n-1 for connectivity");

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<int> perm(n);
    std::iota(perm.begin(), perm.end(), 0);
    std::shuffle(perm.begin(), perm.end(), gen);

    for (int i = 0; i < n - 1; i++) {
        std::uniform_int_distribution<int> parent_dist(0, i);
        int u = perm[i + 1];
        int v = perm[parent_dist(gen)];
        tp.append_edge(u, v);
    }

    std::uniform_int_distribution<int> node_dist(0, n - 1);

    while (tp.m_ < m) {
        int u = node_dist(gen);
        int v = node_dist(gen);
        if (u == v) continue;
        tp.append_edge(u, v);
    }

    return tp;
}

Topology generateSparse(int n){

    Topology tp(n, 0);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<std::vector<std::pair<int, int>>> adj(n);
    for (int i = 1; i < n; ++i) {
        std::uniform_int_distribution<int> dist(0, i - 1);
        int p = dist(gen);
        tp.append_edge(i, p);
        adj[i].emplace_back(i, p);
        adj[p].emplace_back(i, p);
    }

    int sparse_target = (3 * n) / 2;
    std::uniform_int_distribution<int> node_dist(0, n - 1);

    while (tp.m_ < sparse_target) {
        int u = node_dist(gen);
        int v = node_dist(gen);
        int cnt = tp.m_;
        if (u == v) continue;
        tp.append_edge(u, v);
        if (tp.m_ > cnt){
            adj[u].emplace_back(u, v);
            adj[v].emplace_back(u, v);
        }
    }

    std::vector<int> perm(n);
    iota(perm.begin(), perm.end(), 0);
    shuffle(perm.begin(), perm.end(), gen);


    size_t idx = 0;
    while (tp.m_ < 5 * n) {
        int u = perm[idx % n];
        idx++;

        if (adj[u].empty()) continue;
        std::uniform_int_distribution<int> d1(0, (int)adj[u].size() - 1);
        auto pos = d1(gen);
        int v = (adj[u][pos].first == u ? adj[u][pos].second : adj[u][pos].first);

        if (adj[v].empty()) continue;

        std::uniform_int_distribution<int> d2(0, (int)adj[v].size() - 1);
        pos = d2(gen);
        int w = (adj[v][pos].first == v ? adj[v][pos].second : adj[v][pos].first);

        int cnt = tp.m_;
        if (u == w) continue;
        tp.append_edge(u, w);
        if (tp.m_ > cnt){
            adj[u].emplace_back(u, w);
            adj[w].emplace_back(u, w);
        }
    }

    return tp;
}