//
// Created by Sergei Kudria on 2026/1/25.
//

#include "plot.h"

pybind11::scoped_interpreter guard{};

void plot_p_eta(
        const std::string& name,
        const std::vector<double>& p_mul,
        const std::vector<std::vector<double>>& r_bal,
        const std::string& save_path,
        bool show
) {
    auto package_path = std::string(MAIN_DIR) + "stat/";
    pybind11::module_ sys = pybind11::module_::import("sys");
    sys.attr("path").attr("insert")(0, package_path);
    pybind11::module_ plot = pybind11::module_::import("plot_p_eta");
    plot.attr("plot_rows")(name.substr(0, name.size() - 4), r_bal, p_mul, save_path, show);
}

void plot_p_eta_legend(
        const std::vector<double>& values,
        const std::string& save_path
){
    auto package_path = std::string(MAIN_DIR) + "stat/";
    pybind11::module_ sys = pybind11::module_::import("sys");
    sys.attr("path").attr("insert")(0, package_path);
    pybind11::module_ plot = pybind11::module_::import("plot_p_eta_legend");
    plot.attr("plot_eta_legend")(values, save_path);
}

void plot_CI(
        const prefix_CI& ci_naive,
        const prefix_CI& ci_rb,
        const std::string& save_path,
        bool show
) {
    auto package_path = std::string(MAIN_DIR) + "stat/";
    pybind11::module_ sys = pybind11::module_::import("sys");
    sys.attr("path").attr("insert")(0, package_path);
    pybind11::module_ plot = pybind11::module_::import("plot_ci");

    auto& mean1 = ci_naive.mean_;
    auto& lb1 = ci_naive.lb_;
    auto& ub1 = ci_naive.ub_;
    auto& mean2 = ci_rb.mean_;
    auto& lb2 = ci_rb.lb_;
    auto& ub2 = ci_rb.ub_;
    auto delta = ci_naive.delta_;

    plot.attr("plot_comparison")(mean1, lb1, ub1, mean2, lb2, ub2, delta, save_path, show);
}