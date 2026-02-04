//
// Created by HighlySkilledCoder on 2026/1/24.
//

#include "generator.h"

void genSyntheticSparse(int n, const std::string& file, double p_pos){
    auto tp = generateSparse(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution e_pos(p_pos);
    std::vector<bool> s(tp.m_);
    std::vector<double> dist(tp.m_);
    std::uniform_real_distribution pos_prob(0.8, 1.0);
    std::uniform_real_distribution neg_prob(4.0 / (tp.m_), 32.0 / tp.m_);
    for (int i = 0; i < tp.m_; ++i){
        s[i] = e_pos(gen);
        if (s[i]) dist[i] = pos_prob(gen);
        else dist[i] = neg_prob(gen);
    }

    std::ofstream f(file);
    f << std::setprecision(3);
    f << tp.n_ << '\t' << tp.m_ << '\n';
    for (int i = 0; i < tp.m_; ++i)
        f << tp.head_[i] << '\t' << tp.tail_[i] << '\t' << s[i] << '\t' << dist[i] << std::endl;

    f.close();
}

std::vector<std::pair<int, int>> genSyntheticNearlyBalanced(NearlyBalancedParams params, const std::string& file){
    std::random_device rd;
    std::mt19937 gen(rd());
    int n1 = params.n1_, n2 = params.n2_, m1 = params.m1_, m2 = params.m2_,
            btw = params.btw_, c1 = params.c1_, c2 = params.c2_;
    int n = n1 + n2, m = m1 + m2 + btw + c1 + c2;

    std::vector<bool> s(m, false);
    std::fill(s.begin(), s.begin() + m1 + m2, true);
    std::vector<std::bernoulli_distribution> p_(m);
    std::uniform_real_distribution pos_dist(0.8, 1.0);
    std::uniform_real_distribution neg_dist(4.0 / ((double) (m1 + m2)), 16.0 / ((double) (m1 + m2)));
    for (int eid = 0; eid < m1 + m2; ++eid) p_[eid] = std::bernoulli_distribution(pos_dist(gen));
    for (int eid = m1 + m2; eid < m; ++eid) p_[eid] = std::bernoulli_distribution(neg_dist(gen));

    Topology t = generateRandom(n1, m1);
    Topology t2 = generateRandom(n2, m2);
    t.join(t2);

    std::uniform_int_distribution dist1(0, n1 - 1);
    std::uniform_int_distribution dist2(n1, n1 + n2 - 1);
    while (t.m_ < m1 + m2 + btw){
        int i = dist1(gen), j = dist2(gen);
        if (i == j) continue;
        t.append_edge(i, j);
    }

    std::vector<std::pair<int, int>> f_ed;
    while (t.m_ < m - c2){
        int i = dist1(gen), j = dist1(gen);
        if (i == j) continue;
        t.append_edge(i, j);
        f_ed.emplace_back(i, j);
    }

    while (t.m_ < m){
        int i = dist2(gen), j = dist2(gen);
        if (i == j) continue;
        t.append_edge(i, j);
        f_ed.emplace_back(i, j);
    }

    std::ofstream f(file);
    f << std::setprecision(3);
    f << n << '\t' << m << std::endl;
    for (auto eid = 0; eid < m; ++eid)
        f << t.head_[eid] << '\t' << t.tail_[eid] << '\t' << s[eid] << '\t' << p_[eid].p() << std::endl;
    f.close();

    std::vector<std::pair<int, int>> ans;
    ans.reserve(f_ed.size());
    for (auto& fed: f_ed) ans.emplace_back(fed.first, fed.second);
    return ans;
}

