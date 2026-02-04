//
// Created by HighlySkilledCoder on 2026/1/30.
//

#ifndef BR_USG_EXPERIMENTS_H
#define BR_USG_EXPERIMENTS_H

#include "core/algo.h"
#include "utils.h"

/**
 * @brief Compare naive Monte Carlo vs Rao-Blackwellized estimator efficiency.
 *
 * Uses instances in data/synthetic_sparse. If plot_dir is non-empty, confidence
 * interval plots are saved there.
 */
void run_efficiency_experiment(const std::string& plot_dir, int num_threads, int n_samples = 1000, double delta = 0.05);

/**
 * @brief Sweep p-multipliers and eta values on real-world balanced graphs.
 *
 * Uses instances in data/real_world_balanced and parameters from eta_list and
 * p_factor_list in the project root.
 */
void run_p_eta_experiment(const std::string& plot_dir, int num_threads, int n_samples = 1000);

/**
 * @brief Inject a cross-edge into balanced instances and try to identify it.
 *
 * Uses instances in data/real_world_balanced and reports the best edge removal.
 */
void run_cross_edge_experiment(int num_threads, int n_samples = 1000);

#endif //BR_USG_EXPERIMENTS_H
