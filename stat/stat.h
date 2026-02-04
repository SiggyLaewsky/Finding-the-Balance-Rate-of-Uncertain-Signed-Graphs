//
// Created by Sergei Kudria on 2026/1/24.
//

#ifndef BR_USG_STAT_H
#define BR_USG_STAT_H

#include <vector>

/// Empirical variance for a single-sample vector.
double empirical_variance(const std::vector<double>& res);
/// Empirical variance for a product estimator represented by per-component samples.
double empirical_variance(const std::vector<std::vector<double>>& res);

/// Prefix confidence intervals for running means.
struct prefix_CI{
    std::vector<double> mean_;  ///< Prefix means.
    std::vector<double> lb_;    ///< Lower bounds.
    std::vector<double> ub_;    ///< Upper bounds.
    double delta_;              ///< Confidence level parameter.
    std::string descr_;         ///< Human-readable description.
};

/// Hoeffding-based prefix confidence intervals for a Bernoulli mean.
prefix_CI get_hoeffding_CI(const std::vector<double>& r, double delta);

/// Log-normal CI for a product estimator from component-wise samples.
prefix_CI get_log_delta_CI(const std::vector<std::vector<double>>& r, double delta);


#endif //BR_USG_STAT_H
