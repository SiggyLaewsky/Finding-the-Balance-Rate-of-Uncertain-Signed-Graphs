//
// Created by Sergei Kudria on 2026/1/23.
//

#include "experiments.h"
#include "core/algo.h"
#include "stat/plot.h"

int main(){

    std::string plot_dir(DATA_DIR);
    run_p_eta_experiment();
    //run_efficiency_experiment(plot_dir);
/*    const std::string fname(DATA_DIR);
    Graph gr(fname + "real_world_balanced/wikipolitics.txt");
    int n_smp = 1000;
    auto dec = gr.preprocess();

    auto res = naive_MC_sample(gr, n_smp);
    auto ci = get_hoeffding_CI(res, 0.05);
    plot_CI(ci, fname + "ans.png");

    auto res2 = rao_blackwell_sample(gr, n_smp);
    auto ci2 = get_log_delta_CI(res2, 0.05);
    plot_CI(ci2, fname + "ans1.png");
    plot_naive_vs_rb(ci, ci2, fname + "ans2.svg");*/

/*
    auto rb = rao_blackwell_sample(gr, n_smp);
    auto r_mean = std::accumulate(rb.begin(), rb.end(), 0.0) / ((double) n_smp);
    std::cout << r_mean << std::endl;

    auto db = naive_MC_sample(gr, n_smp);
    auto d_mean = std::accumulate(db.begin(), db.end(), 0.0) / ((double) n_smp);
    std::cout << d_mean << std::endl;

    auto ans = find_FES(gr, 1000, 0.001);
    for (auto& ed: ans)
        std::cout << ed.first << '\t' << ed.second << std::endl;
*/

}