//
// Created by Sergei Kudria on 2026/1/25.
//

#include <format>
#include "plot.h"

pybind11::scoped_interpreter guard{};

pybind11::array_t<double> to_np(const std::vector<double>& v) {
    return pybind11::array_t<double>(v.size(), v.data());
}

void plot_CI(prefix_CI ci_data, const std::string& filename){
    pybind11::module plt = pybind11::module::import("matplotlib.pyplot");

    pybind11::dict rc;
    rc["text.usetex"]     = true;
    rc["font.family"]     = "serif";
    rc["font.size"]       = 12;
    rc["axes.labelsize"]  = 12;
    rc["axes.titlesize"]  = 13;
    rc["legend.fontsize"] = 11;
    rc["figure.dpi"]      = 150;
    rc["axes.grid"]       = true;
    rc["grid.alpha"]      = 0.3;

    plt.attr("rcParams").attr("update")(rc);

    pybind11::tuple res = plt.attr("subplots")(pybind11::arg("figsize") = pybind11::make_tuple(8, 5));
    pybind11::object fig = res[0];
    pybind11::object ax  = res[1];

    std::vector<double> tc(ci_data.mean_.size());
    std::iota(tc.begin(), tc.end(), 1);

    auto t = to_np(tc);

    pybind11::array_t<double> xmean = to_np(ci_data.mean_);
    pybind11::array_t<double> hl    = to_np(ci_data.lb_);
    pybind11::array_t<double> hu    = to_np(ci_data.ub_);

    ax.attr("plot")(
            t, xmean,
            pybind11::arg("color") = "black",
            pybind11::arg("lw")    = 1.8,
            pybind11::arg("label") = R"(Empirical mean $\hat{p}_t$)"
    );

    ax.attr("fill_between")(
            t, hl, hu,
            pybind11::arg("color") = "blue",
            pybind11::arg("alpha") = 0.25,
            pybind11::arg("label") = "Hoeffding prefix_CI"
    );


    ax.attr("set_xlabel")(R"(Sample size $t$)");
    ax.attr("set_ylabel")("Mean");

    ax.attr("set_title")(ci_data.descr_);

    ax.attr("set_ylim")(-0.02, 1.02);

    ax.attr("legend")(pybind11::arg("frameon") = false);

    ax.attr("set_title")(
            "Hoeffding vs LRT Confidence Intervals, $\\delta=" +
                    std::format("{:.2f}", ci_data.delta_) + "$"
    );


    plt.attr("tight_layout")();
    if (!filename.empty()) plt.attr("savefig")(filename);
}

void plot_naive_vs_rb(const prefix_CI& ci_naive, const prefix_CI& ci_rb, const std::string& filename){
    pybind11::module plt = pybind11::module::import("matplotlib.pyplot");

    pybind11::dict rc;
    rc["text.usetex"]     = true;
    rc["font.family"]     = "serif";
    rc["font.size"]       = 12;
    rc["axes.labelsize"]  = 12;
    rc["axes.titlesize"]  = 13;
    rc["legend.fontsize"] = 11;
    rc["figure.dpi"]      = 150;
    rc["axes.grid"]       = true;
    rc["grid.alpha"]      = 0.3;

    plt.attr("rcParams").attr("update")(rc);

    pybind11::tuple res = plt.attr("subplots")(pybind11::arg("figsize") = pybind11::make_tuple(8, 5));
    pybind11::object fig = res[0];
    pybind11::object ax  = res[1];

    std::vector<double> tc(ci_naive.mean_.size());
    std::iota(tc.begin(), tc.end(), 1);

    auto t = to_np(tc);

    pybind11::array_t<double> hmean = to_np(ci_naive.mean_);
    pybind11::array_t<double> hl    = to_np(ci_naive.lb_);
    pybind11::array_t<double> hu    = to_np(ci_naive.ub_);
    pybind11::array_t<double> rmean = to_np(ci_rb.mean_);
    pybind11::array_t<double> rl    = to_np(ci_rb.lb_);
    pybind11::array_t<double> ru    = to_np(ci_rb.ub_);

    ax.attr("plot")(
            t, hmean,
            pybind11::arg("color") = "blue",
            pybind11::arg("lw")    = 0.8,
            pybind11::arg("label") = R"(Naive, historical $\hat{R}_{bal}$)"
    );

    ax.attr("plot")(
            t, rmean,
            pybind11::arg("color") = "black",
            pybind11::arg("lw")    = 0.8,
            pybind11::arg("label") = R"(Proposed, historical $\hat{R}_{bal}$)"
    );

    ax.attr("fill_between")(
            t, hl, hu,
            pybind11::arg("color") = "blue",
            pybind11::arg("alpha") = 0.25,
            pybind11::arg("label") = "Hoeffding prefix_CI"
    );

    ax.attr("fill_between")(
            t, rl, ru,
            pybind11::arg("color") = "red",
            pybind11::arg("alpha") = 0.25,
            pybind11::arg("label") = "Proposed, delta prefix_CI"
    );


    ax.attr("set_xlabel")(R"(Sample size $t$)");
    ax.attr("set_ylabel")("Mean");

    ax.attr("set_ylim")(-0.02, 1.02);

    ax.attr("legend")(pybind11::arg("frameon") = false);

    ax.attr("set_title")(
            "Effect of Rao-Blackwellization and decomposition, $\\delta=" +
            std::format("{:.2f}", ci_naive.delta_) + "$"
    );

    plt.attr("tight_layout")();
    if (!filename.empty()) plt.attr("savefig")(filename);
}
