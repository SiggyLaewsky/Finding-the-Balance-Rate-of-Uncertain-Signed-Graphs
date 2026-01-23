//
// Created by Sergei Kudria on 2026/1/30.
//

#include <regex>
#include <random>
#include <filesystem>
#include <iostream>
#include "stat/plot.h"
#include "experiments.h"

void run_efficiency_experiment(const std::string& plot_dir, int num_samples, double delta){
    std::vector<std::string> instances;
    std::string sparse_dir = std::string(DATA_DIR) + "synthetic_sparse/";
    std::regex sparse_pattern(R"(^sparse_[1-9][0-9]*\.txt$)");
    try{
        for (const auto& entry: std::filesystem::directory_iterator(sparse_dir)){
            auto fname = entry.path().filename().string();
            if (std::regex_match(fname, sparse_pattern))
                instances.push_back(fname);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    std::cout << "Found " << instances.size() << " sparse instances." << std::endl;

    std::sort(instances.begin(), instances.end(), [](auto& a, auto& b) {
        return std::stoll(a.substr(7)) < std::stoll(b.substr(7));
    });

    for (auto& instance: instances){
        std::cout << "Instance " << instance << ":\n";
        Graph gr(sparse_dir + instance);
        std::cout << "|V| = " << gr.n_ << "\t|V| = " << gr.m_ << std::endl;

        auto naive_st = std::chrono::steady_clock::now();
        auto res_naive = naive_MC_sample(gr, num_samples);
        auto naive_fn = std::chrono::steady_clock::now();
        auto naive_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(naive_fn - naive_st).count();
        std::cout << "Naive MC sampling took: " << naive_elapsed
             << " ms. Empirical variance: " <<
            empirical_variance(res_naive) << std::endl;

        auto rb_st = std::chrono::steady_clock::now();
        auto res_rb = rao_blackwell_sample(gr, num_samples);
        std::cout << "Instance is decomposed into " << res_rb.size() << " components.\n";
        auto rb_fn = std::chrono::steady_clock::now();
        auto rb_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(rb_fn - rb_st).count();
        std::cout << "Rao-Blackwellized sampling with preprocessing took: " << rb_elapsed
                  << " ms. Empirical variance: " <<
                  empirical_variance(res_rb) << std::endl;

        auto instance_name = instance.substr(0, instance.size() - 4);

        auto ci_hf = get_hoeffding_CI(res_naive, delta);
        auto ci_rb = get_log_delta_CI(res_rb, delta);
        plot_naive_vs_rb(ci_hf, ci_rb, plot_dir + instance_name + "_comparison.svg");
    }
}

void run_p_eta_experiment(int n_samples){
    std::string data_dir = std::string(DATA_DIR) + "real_world_balanced/";
    std::vector<std::string> instances;
    try{
        for (const auto& entry: std::filesystem::directory_iterator(data_dir)){
            auto fname = entry.path().filename().string();
            instances.push_back(fname);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    std::cout << "Found " << instances.size() << " sparse instances." << std::endl;

    std::vector<double> eta_list = {0.05 * 1e-2, 0.1 * 1e-2, 0.5 * 1e-2, 1e-2, 2*1e-2, 5*1e-2};
    std::vector<double> pmul_list = {1.0, 1.5, 2.5, 5.0, 10.0};
    std::random_device rd;
    std::mt19937 gen(rd());
    for (auto& instance: instances) {
        std::cout << "Instance " << instance << ":\n";
        Graph gr(data_dir + instance);
        std::cout << "|V| = " << gr.n_ << "\t|V| = " << gr.m_ << std::endl;
        double rng = std::log((double) gr.m_);
        for (auto eid = 0; eid < gr.m_; ++eid){
            std::uniform_real_distribution pmean_dist(0.01 / rng, 1.0 / rng);
            gr.dist_[eid] = std::bernoulli_distribution(pmean_dist(gen));
        }


        std::unordered_set<long long> used;
        auto encode = [](int u, int v) {
            if (u > v) std::swap(u, v);
            return (long long)u << 32 | v;
        };
        for (auto eid = 0; eid < gr.m_; ++eid) used.insert(encode(gr.head_[eid], gr.tail_[eid]));


        for (auto eta: eta_list){
            for (auto pmul: pmul_list){
                auto gr1 = gr;
                auto used1 = used;

                int m_add = eta * gr.m_;
                for (auto ed = 0; ed < m_add; ++ed){
                    std::uniform_int_distribution v_dist(0, gr.n_ - 1);
                    std::bernoulli_distribution sign_dist(0.5);
                    std::uniform_real_distribution pmean_dist(0.01 / rng, 1.0 / rng);

                    int u = 0, v = 0;
                    while (u == v || used1.find(encode(u, v)) != used1.end()){
                        u = v_dist(gen);
                        v = v_dist(gen);
                    }
                    used1.insert(encode(u, v));

                    gr1.head_.push_back(u);
                    gr1.tail_.push_back(v);
                    gr1.s_.push_back(sign_dist(gen));
                    gr1.dist_.emplace_back(pmean_dist(gen));
                }

                gr1.m_ += m_add;
                for (auto eid = 0; eid < gr1.m_; ++eid) {
                    double pe = gr1.dist_[eid].p();
                    gr1.dist_[eid] = std::bernoulli_distribution(std::min(1.0, pe * pmul));
                }

                std::cout << eval_rbal(gr1, n_samples) << '\t';
            }
            std::cout << std::endl;
        }
    }
}