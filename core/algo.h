//
// Created by Sergei Kudria on 2026/1/23.
//

#ifndef BR_USG_ALGO_H
#define BR_USG_ALGO_H

#include "graph.h"
#include "dsu.h"

std::vector<double> naive_MC_sample(Graph& gr, int n_samples, int num_threads);
std::vector<double> rao_blackwell_sample_single(Graph& gr, int n_samples, int num_threads);
std::vector<std::vector<double>> rao_blackwell_sample(Graph& gr, int n_samples, int num_threads);

double eval_rbal(Graph& gr, int n_samples, int num_threads);
std::vector<std::pair<int, int>> find_FES(Graph& gr, int n_samples, double delta);

#endif //BR_USG_ALGO_H
