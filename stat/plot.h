//
// Created by Sergei Kudria on 2026/1/25.
//

#ifndef BR_USG_PLOT_H
#define BR_USG_PLOT_H

#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <vector>
#include "stat.h"

void plot_p_eta(
        const std::string& name,
        const std::vector<double>& p_mul,
        const std::vector<std::vector<double>>& r_bal,
        const std::string& save_path,
        bool show = false
);

void plot_p_eta_legend(
        const std::vector<double>& values,
        const std::string& save_path
);


void plot_CI(
        const prefix_CI& ci_naive,
        const prefix_CI& ci_rb,
        const std::string& save_path,
        bool show = false
);

#endif //BR_USG_PLOT_H
