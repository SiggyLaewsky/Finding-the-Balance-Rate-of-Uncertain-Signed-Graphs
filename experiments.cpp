//
// Created by HighlySkilledCoder on 2026/1/30.
//

#include <iostream>
#include "stat/plot.h"
#include "experiments.h"

void run_efficiency_experiment(const std::string& plot_dir, int num_threads, int num_samples, double delta){
    auto source_dir = std::string(DATA_DIR) + "synthetic_sparse/";
    auto instances = get_dir_files(source_dir);

    for (auto& instance: instances){
        auto instance_name = instance.substr(0, instance.size() - 4);
        std::cout << "Instance " << instance_name << ":\n";
        Graph gr(source_dir + instance);
        std::cout << "|V| = " << gr.n_ << "\t|E| = " << gr.m_ << std::endl;

        auto naive_st = std::chrono::steady_clock::now();
        auto res_naive = naive_MC_sample(gr, num_samples, num_threads);
        auto naive_fn = std::chrono::steady_clock::now();
        auto naive_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(naive_fn - naive_st).count();
        std::cout << "Naive MC sampling took: " << naive_elapsed
             << " ms. Empirical variance: " <<
            empirical_variance(res_naive) << std::endl;

        auto rb_st = std::chrono::steady_clock::now();
        auto res_rb = rao_blackwell_sample(gr, num_samples, num_threads);
        std::cout << "Instance is decomposed into " << res_rb.size() << " components.\n";
        auto rb_fn = std::chrono::steady_clock::now();
        auto rb_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(rb_fn - rb_st).count();
        std::cout << "Rao-Blackwellized sampling with preprocessing took: " << rb_elapsed
                  << " ms. Empirical variance: " <<
                  empirical_variance(res_rb) << std::endl;

        if (!plot_dir.empty()) {
            auto ci_hf = get_hoeffding_CI(res_naive, delta);
            auto ci_rb = get_log_delta_CI(res_rb, delta);
            plot_CI(ci_hf, ci_rb, plot_dir + instance_name + "_comparison.pdf");
        }
    }
}

void run_p_eta_experiment(const std::string& plot_dir, int num_threads, int n_samples){
    auto source_dir = std::string(DATA_DIR) + "real_world_balanced/";
    auto instances = get_dir_files(source_dir);
    auto eta_list = read_params<double>(std::string(MAIN_DIR) + "eta_list");
    auto pmul_list = read_params<double>(std::string(MAIN_DIR) + "p_factor_list");
    std::sort(eta_list.begin(), eta_list.end());
    std::sort(pmul_list.begin(), pmul_list.end());

    for (auto& instance: instances) {
        auto instance_name = instance.substr(0, instance.size() - 4);
        std::cout << "Instance " << instance_name << ":\n";
        Graph gr(source_dir + instance);
        std::cout << "|V| = " << gr.n_ << "\t|E| = " << gr.m_ << std::endl;

        std::random_device rd;
        std::mt19937 gen(rd());
        int m_orig = gr.m_;
        double rng = std::log((double) gr.m_);
        std::vector<double> p_orig(gr.m_);
        for (auto eid = 0; eid < gr.m_; ++eid){
            std::uniform_real_distribution pmean_dist(0.01 / rng, 1.0 / rng);
            gr.dist_[eid] = std::bernoulli_distribution(pmean_dist(gen));
            p_orig[eid] = gr.dist_[eid].p();
        }

        std::unordered_set<long long> used;
        auto encode = [](int u, int v) {
            if (u > v) std::swap(u, v);
            return (long long)u << 32 | v;
        };
        for (auto eid = 0; eid < gr.m_; ++eid) used.insert(encode(gr.head_[eid], gr.tail_[eid]));
        std::vector<std::vector<double>> res(0);

        for (auto eta: eta_list){

            // append edges
            auto m_add = -gr.m_ + (int) ((1.0 + eta) * m_orig);
            for (auto ed = 0; ed < m_add; ++ed){
                std::uniform_int_distribution v_dist(0, gr.n_ - 1);
                std::bernoulli_distribution sign_dist(0.5);
                std::uniform_real_distribution pmean_dist(0.01 / rng, 1.0 / rng);

                int u = 0, v = 0;
                while (u == v || used.find(encode(u, v)) != used.end()){
                    u = v_dist(gen);
                    v = v_dist(gen);
                }
                used.insert(encode(u, v));

                gr.head_.push_back(u);
                gr.tail_.push_back(v);
                gr.s_.push_back(sign_dist(gen));
                double pe = pmean_dist(gen);
                p_orig.push_back(pe);
                gr.dist_.emplace_back(pe);
            }
            gr.m_ += m_add;

            std::vector<double> rbal_loc(0);
            for (auto pmul: pmul_list){

                // rescale probabilities
                for (auto eid = 0; eid < gr.m_; ++eid) {
                    double pe = p_orig[eid];
                    gr.dist_[eid] = std::bernoulli_distribution(std::min(1.0, pe * pmul));
                }
                double rbal = eval_rbal(gr, n_samples, num_threads);
                rbal_loc.emplace_back(rbal);
                std::cout << rbal << '\t';
            }

            res.emplace_back(rbal_loc);
            std::cout << std::endl;
        }

        if (!plot_dir.empty())
            plot_p_eta(instance, pmul_list, res, plot_dir + instance_name + ".pdf");
    }

    if (!plot_dir.empty())
        plot_p_eta_legend(eta_list, plot_dir + "eta_legend.pdf");
}


void run_cross_edge_experiment(int num_threads, int n_samples){
    int num_edge2scan = 10;
    auto real_dir = std::string(DATA_DIR) + "real_world_balanced/";
    auto real_instances = get_dir_files(real_dir);

    auto run_instance = [&](const std::string& filepath){
        Graph gr(filepath);
        std::cout << "|V| = " << gr.n_ << "\t|E| = " << gr.m_ << std::endl;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution p_dist(0.5, 1.0);
        std::uniform_int_distribution v_dist(0, gr.n_ - 1);

        DSU dsu(gr.n_);
        std::unordered_set<long long> used;
        auto encode = [](int u, int v) {
            if (u > v) std::swap(u, v);
            return (long long)u << 32 | v;
        };

        for (auto eid = 0; eid < gr.m_; ++eid){
            used.insert(encode(gr.head_[eid], gr.tail_[eid]));
            auto unite = dsu.unite(gr.head_[eid], gr.tail_[eid], gr.s_[eid]);
            if (!unite) throw std::runtime_error("Instance " + filepath + " is not balanced");
            gr.dist_[eid] = std::bernoulli_distribution(p_dist(gen));
        }

        // pick a new edge
        int u = -1, v = -1;
        int cnt = 0;
        bool sgn;
        while (cnt++ < 1e4) {
            while (u == v || used.find(encode(u, v)) != used.end()) {
                u = v_dist(gen);
                v = v_dist(gen);
            }
            auto p1 = dsu.find(u), p2 = dsu.find(v);
            if (p1.second != p2.second) continue;
            sgn = (p1.first ^ p2.first);
            break;
        }
        if (cnt >= 999) throw std::runtime_error("Instance " + filepath + ": failed to find a cross-edge");

        std::cout << "Injected cross-edge: " << u << " - " << v << std::endl;
        gr.m_++;
        gr.head_.push_back(u);
        gr.tail_.push_back(v);
        gr.dist_.emplace_back(p_dist(gen));
        gr.s_.push_back(sgn);
        auto unite = dsu.unite(u, v, sgn);
        if (unite) throw std::runtime_error("Unknown error");

        std::vector<int> edge2scan;
        if (gr.m_ <= num_edge2scan) {
            edge2scan.resize(gr.m_);
            std::iota(edge2scan.begin(), edge2scan.end(), 0);
        } else{
            std::vector<int> pool(gr.n_);
            std::iota(pool.begin(), pool.end(), 0);

            edge2scan.reserve(num_edge2scan);

            std::mt19937 rng(std::random_device{}());
            std::sample(pool.begin(), pool.end(), std::back_inserter(edge2scan), num_edge2scan, rng);
        }

        edge2scan.push_back(gr.m_ - 1);
        auto head_orig = gr.head_;
        auto tail_orig = gr.tail_;

        double max_rbal = 0.0;
        int eid_argmax = -1;
        auto m_orig = gr.m_;
        for (auto& eid: edge2scan){
            int u_ed = gr.head_[eid], v_ed = gr.tail_[eid];
            gr.remove_edge(u_ed, v_ed);
            auto r_bal = eval_rbal(gr, n_samples, num_threads);
            if (r_bal > max_rbal){
                max_rbal = r_bal;
                eid_argmax = eid;
            }
            gr.m_++;
            std::swap(gr.head_[eid], gr.head_[m_orig - 1]);
            std::swap(gr.tail_[eid], gr.tail_[m_orig - 1]);
            std::swap(gr.s_[eid], gr.s_[m_orig - 1]);
            std::swap(gr.dist_[eid], gr.dist_[m_orig - 1]);
        }
        u = gr.head_[eid_argmax];
        v = gr.tail_[eid_argmax];
        std::cout << "Maximum balance rate: " << max_rbal << " on removing edge " << u << " - " << v
            << "\nIs balanced after removal: ";

        gr.remove_edge(u, v);
        dsu.reset();
        for (auto eid = 0; eid < gr.m_; ++eid){
            unite = dsu.unite(gr.head_[eid], gr.tail_[eid], gr.s_[eid]);
            if (!unite) break;
        }
        std::cout << unite << std::endl;
        if (!unite) throw std::runtime_error("Failed to identify the cross-edge");

    };

    for (auto& instance: real_instances){
        auto instance_name = instance.substr(0, instance.size() - 4);
        std::cout << "Instance " << instance_name << ":\n";
        run_instance(real_dir + instance);
        std::cout << std::endl;
    }
}