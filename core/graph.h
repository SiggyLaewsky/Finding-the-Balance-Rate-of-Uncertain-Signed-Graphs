//
// Created by Sergei Kudria on 2026/1/23.
//

#ifndef BR_USG_GRAPH_H
#define BR_USG_GRAPH_H

#include <vector>
#include <random>
#include <string>
#include <chrono>

struct Graph{

    Graph();
    explicit Graph(const std::string& filename);
    Graph(int n, int m, std::vector<int>& i, std::vector<int>& j, std::vector<double>& p, std::vector<bool>& s);
    std::vector<Graph> preprocess();
    void remove_edge(int i, int j);
    void return_edge(int i, int j);
    void save(const std::string& filename);

    int n_ = 0;
    int m_ = 0;
    std::vector<int> head_;
    std::vector<int> tail_;
    std::vector<bool> s_;
    std::vector<std::bernoulli_distribution> dist_;
};

#endif //BR_USG_GRAPH_H
