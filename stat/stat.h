//
// Created by Sergei Kudria on 2026/1/24.
//

#ifndef BR_USG_STAT_H
#define BR_USG_STAT_H

#include <vector>

double empirical_variance(const std::vector<double>& res);
double empirical_variance(const std::vector<std::vector<double>>& res);

// structure for saving prefix confidence intervals
struct prefix_CI{
    std::vector<double> mean_;
    std::vector<double> lb_;
    std::vector<double> ub_;
    double delta_;
    std::string descr_;
};

prefix_CI get_hoeffding_CI(const std::vector<double>& r, double delta);

prefix_CI get_log_delta_CI(const std::vector<std::vector<double>>& r, double delta);


#endif //BR_USG_STAT_H
