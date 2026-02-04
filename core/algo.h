//
// Created by Sergei Kudria on 2026/1/23.
//

#ifndef BR_USG_ALGO_H
#define BR_USG_ALGO_H

#include "graph.h"
#include "dsu.h"

/**
 * @brief Plain Monte Carlo sampling of the balance indicator.
 * @return Vector of size n_samples with 0/1 outcomes per sample.
 */
std::vector<double> naive_MC_sample(Graph& gr, int n_samples, int num_threads);
/**
 * @brief Rao-Blackwellized sampling for a single (non-decomposed) component.
 * @return Vector of size n_samples with per-sample balance probability estimates.
 */
std::vector<double> rao_blackwell_sample_single(Graph& gr, int n_samples, int num_threads);
/**
 * @brief Rao-Blackwellized sampling after preprocessing into components.
 * @return One sample vector per component.
 */
std::vector<std::vector<double>> rao_blackwell_sample(Graph& gr, int n_samples, int num_threads);

/**
 * @brief Evaluate the balance rate using Rao-Blackwellized sampling.
 */
double eval_rbal(Graph& gr, int n_samples, int num_threads);
/**
 * @brief Greedy search for an edge set whose removal increases balance rate.
 *
 * The algorithm repeatedly removes the single edge that yields the largest
 * increase in estimated balance rate until the estimate reaches 1 - delta
 * or no further improvement is found.
 */
std::vector<std::pair<int, int>> find_FES(Graph& gr, int n_samples, double delta, int num_threads = 1);

#endif //BR_USG_ALGO_H
