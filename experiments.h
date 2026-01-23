//
// Created by Sergei Kudria on 2026/1/30.
//

#ifndef BR_USG_EXPERIMENTS_H
#define BR_USG_EXPERIMENTS_H

#include "core/algo.h"

void run_efficiency_experiment(const std::string& plot_dir, int n_samples = 1000, double delta = 0.05);

void run_p_eta_experiment(int n_samples = 1000);

#endif //BR_USG_EXPERIMENTS_H
