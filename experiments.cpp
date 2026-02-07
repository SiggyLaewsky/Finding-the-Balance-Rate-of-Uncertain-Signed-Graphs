

#include <iostream>
#include "stat/plot.h"
#include "experiments.h"

void run_efficiency_experiment(const std::string& plot_dir, int num_threads, int num_samples, double delta){

    auto run_instance = [&](std::string& source_dir, std::string& instance){
        auto instance_name = instance.substr(0, instance.size() - 4);
        std::cout << "Instance " << instance_name << ":\n";
        Graph gr(source_dir + instance);
        std::cout << "|V| = " << gr.n_ << "\t|E| = " << gr.m_ << std::endl;
        gr.set_p_logarithmic();

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

        std::cout << "evaluated balanced rate: " << eval_rbal(gr, num_samples, num_threads) << std::endl;

        if (!plot_dir.empty()) {
            auto ci_hf = get_hoeffding_CI(res_naive, delta);
            auto ci_rb = get_log_delta_CI(res_rb, delta);
            plot_CI(ci_hf, ci_rb, plot_dir + instance_name + "_comparison.pdf");
        }
    };

    auto synth_dir = std::string(DATA_DIR) + "synthetic_sparse/";
    auto synth_instances = get_dir_files(synth_dir);
    auto real_dir = std::string(DATA_DIR) + "real_world_orig/";
    auto real_instances = get_dir_files(real_dir);
    for (auto& instance: synth_instances){
        if (instance.substr(instance.size() - 4) != ".txt") continue;
        run_instance(synth_dir, instance);
    }
    for (auto& instance: real_instances){
        if (instance.substr(instance.size() - 4) != ".txt") continue;
        run_instance(real_dir, instance);
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

        auto m_orig = gr.m_;
        auto p_orig = gr.set_p_logarithmic();

        std::unordered_set<long long> used;
        auto encode = [](int u, int v) {
            if (u > v) std::swap(u, v);
            return (long long)u << 32 | v;
        };
        for (auto eid = 0; eid < gr.m_; ++eid) used.insert(encode(gr.head_[eid], gr.tail_[eid]));
        std::vector<std::vector<double>> res(0);
        std::random_device rd;
        std::mt19937 gen(rd());
        double rng = std::log((double) m_orig);

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
    int num_edge2scan = 100;
    int n_critical = 5;
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

        // pick critical edges
        std::unordered_set<long long> critical;
        for (int iCrit = 0; iCrit < n_critical; ++iCrit) {
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

            gr.m_++;
            std::cout << "Injected: " << std::min(u, v) << "-" << std::max(u, v) << std::endl;
            critical.insert(encode(u, v));
            used.insert(encode(u, v));
            gr.head_.push_back(u);
            gr.tail_.push_back(v);
            gr.dist_.emplace_back(p_dist(gen));
            gr.s_.push_back(sgn);
            auto unite = dsu.unite(u, v, sgn);
            if (unite) throw std::runtime_error("Unknown error");
        }

        std::unordered_map<long long, int> edge2scan;
        if (gr.m_ <= num_edge2scan) {
            for (auto eid = 0; eid < gr.m_; ++eid) edge2scan[encode(gr.head_[eid], gr.tail_[eid])] = -1;
        } else{
            std::vector<int> pool(gr.n_);
            std::iota(pool.begin(), pool.end(), 0);

            std::mt19937 rng(std::random_device{}());
            std::vector<int> idx;
            std::sample(pool.begin(), pool.end(), std::back_inserter(idx), num_edge2scan, rng);
            for (auto eid: idx) edge2scan[encode(gr.head_[eid], gr.tail_[eid])] = -1;
        }

        for (int iCrit = 0; iCrit < n_critical; ++iCrit)
            edge2scan[encode(gr.head_[gr.m_ - iCrit - 1], gr.tail_[gr.m_ - iCrit - 1])] = -1;

        std::unordered_set<long long> erased;
        for (int iCrit = 0; iCrit < n_critical; ++iCrit) {
            // find current indices
            for (auto eid = 0; eid < gr.m_; ++eid) {
                auto code = encode(gr.head_[eid], gr.tail_[eid]);
                if (edge2scan.find(code) != edge2scan.end())
                    edge2scan[code] = eid;
            }

            double max_rbal = 0.0;
            long long eid_argmax = -1;
            auto m_orig = gr.m_;
            for (auto &code: edge2scan) {
                int u_ed = static_cast<int>(code.first >> 32), v_ed = static_cast<int>(code.first & 0xFFFFFFFFLL);
                gr.remove_edge(u_ed, v_ed);
                auto r_bal = eval_rbal(gr, n_samples, num_threads);
                if (r_bal > max_rbal) {
                    max_rbal = r_bal;
                    eid_argmax = code.second;
                }
                gr.m_++;
                std::swap(gr.head_[code.second], gr.head_[m_orig - 1]);
                std::swap(gr.tail_[code.second], gr.tail_[m_orig - 1]);
                std::swap(gr.s_[code.second], gr.s_[m_orig - 1]);
                std::swap(gr.dist_[code.second], gr.dist_[m_orig - 1]);
            }

            int u = gr.head_[eid_argmax], v = gr.tail_[eid_argmax];
            gr.remove_edge(u, v);
            edge2scan.erase(encode(u, v));
            std::cout << "Erased: " << std::min(u, v) << "-" << std::max(u, v) << std::endl;
            erased.insert(encode(u, v));
        }
        dsu.reset();

        bool unite = true;
        for (auto eid = 0; eid < gr.m_; ++eid){
            unite = dsu.unite(gr.head_[eid], gr.tail_[eid], gr.s_[eid]);
            if (!unite) break;
        }
        std::cout << "Is result balanced: " << unite << std::endl;

        std::size_t cnt = 0;
        for (auto x : critical) cnt += erased.count(x);
        std::cout << "Found edges from picked critical: " << cnt << "/" << n_critical << std::endl;
    };

    for (auto& instance: real_instances){
        if (instance.substr(instance.size() - 4) != ".txt") continue;
        auto instance_name = instance.substr(0, instance.size() - 4);
        std::cout << "Instance " << instance_name << ":\n";
        run_instance(real_dir + instance);
        std::cout << std::endl;
    }
}