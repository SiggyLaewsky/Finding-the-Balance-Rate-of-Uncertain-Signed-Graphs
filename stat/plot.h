//
// Created by Sergei Kudria on 2026/1/25.
//

#ifndef BR_USG_PLOT_H
#define BR_USG_PLOT_H

#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <vector>
#include "stat.h"


pybind11::array_t<double> to_np(const std::vector<double>& v);

void plot_CI(prefix_CI ci_data, const std::string& filename = "");

void plot_naive_vs_rb(const prefix_CI& ci_naive, const prefix_CI& ci_rb, const std::string& filename = "");



#endif //BR_USG_PLOT_H
