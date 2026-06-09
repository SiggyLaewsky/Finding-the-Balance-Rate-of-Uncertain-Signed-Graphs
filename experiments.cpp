
#include "core/algo.h"
#include "core/ebc.h"
#include "core/inject.h"
#include "utils.h"
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
            plot_CI(ci_hf, ci_rb, plot_dir + instance_name + ".pdf");
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

// auxiliary function
void run_br(Graph& gr, InjectedEdges& injectedEdges, int num_samples, int num_threads){
    auto n_crit = injectedEdges.critical_.size();

    std::unordered_set<long long> erased;
    for (size_t iCrit = 0; iCrit < n_crit; ++iCrit) {
        double max_rbal = -1.0;
        long long code_argmax = -1;
        for (auto edge_code : injectedEdges.edge2scan_) {
            if (erased.count(edge_code)) continue;
            int u_ed = static_cast<int>(edge_code >> 32);
            int v_ed = static_cast<int>(edge_code & 0xFFFFFFFFLL);

            gr.remove_edge(u_ed, v_ed);
            auto r_bal = eval_rbal(gr, num_samples, num_threads);
            if (r_bal > max_rbal) {
                max_rbal = r_bal;
                code_argmax = edge_code;
            }

            gr.return_edge(u_ed, v_ed);
        }

        if (code_argmax != -1) {
            int u = static_cast<int>(code_argmax >> 32);
            int v = static_cast<int>(code_argmax & 0xFFFFFFFFLL);

            gr.remove_edge(u, v);
            std::cout << "Erased: " << std::min(u, v) << "-" << std::max(u, v) << std::endl;
            erased.insert(code_argmax);
        }
        else {
            throw std::logic_error("Failed to iterate ove edge2scan");
        }
    }

    DSU dsu(gr.n_);
    bool unite = true;
    for (auto eid = 0; eid < gr.m_; ++eid){
        unite = dsu.unite(gr.head_[eid], gr.tail_[eid], gr.s_[eid]);
        if (!unite) break;
    }
    std::cout << "Is result balanced: " << unite << std::endl;

    std::size_t cnt = 0;
    for (const auto& elem : injectedEdges.critical_) {
        if (erased.count(elem.first)) ++cnt;
    }

    std::cout << "Found edges from picked critical: " << cnt << "/" << n_crit << std::endl;
}


void run_cross_edge_experiment(int num_threads, int n_samples){
    int num_edge2scan = 100;
    int n_critical = 5;
    auto real_dir = std::string(DATA_DIR) + "real_world_balanced/";
    auto real_instances = get_dir_files(real_dir);

    auto run_instance = [&](const std::string& filepath) {
        Graph gr(filepath);
        std::cout << "|V| = " << gr.n_ << "\t|E| = " << gr.m_ << std::endl;
        auto injectedEdges = inject(gr, n_critical, num_edge2scan);
        run_br(gr, injectedEdges, n_samples, num_threads);
    };

    for (auto& instance: real_instances){
        if (instance.substr(instance.size() - 4) != ".txt") continue;
        auto instance_name = instance.substr(0, instance.size() - 4);
        std::cout << "Instance " << instance_name << ":\n";
        run_instance(real_dir + instance);
        std::cout << std::endl;
    }
}


void run_br_vs_ebc(const std::string& filepath, int num_samples, int num_threads, int n_crit, int num_edge2scan){
    Graph gr(filepath);
    std::cout << "|V| = " << gr.n_ << "\t|E| = " << gr.m_ << std::endl;
    auto injected = inject(gr, n_crit, num_edge2scan);
    std::cout << "Running balance-rate-based heuristic: " << std::endl;
    run_br(gr, injected, num_samples, num_threads);
    std::cout << "Running EBC-based heuristic: " << std::endl;
    EBC ebc(filepath);
    inject(ebc, injected);

    auto ebc_res = ebc.rank_subset(injected.edge2scan_, n_crit);

    std::unordered_set<long long> critical;
    for (auto& el: injected.critical_) critical.insert(el.first);

    auto encode = [](int u, int v) {
        if (u > v) std::swap(u, v);
        return (long long) u << 32 | v;
    };

    int cnt = 0;
    for (auto eid: ebc_res){
        int u = ebc.head_[eid], v = ebc.tail_[eid];
        if (critical.count(encode(u, v))) ++cnt;
        std::cout << "Erased: " << std::min(u, v) << "-" << std::max(u, v) << std::endl;
    }

    std::cout << "Found edges from picked critical: " << cnt << "/" << n_crit << std::endl;
}

void run_cross_edge_ebc_comparison_experiment(int num_threads, int n_samples) {
    int num_edge2scan = 100;
    int n_critical = 5;
    auto real_dir = std::string(DATA_DIR) + "real_world_balanced/";
    auto real_instances = get_dir_files(real_dir);

    auto run_instance = [&](const std::string &filepath) {
        run_br_vs_ebc(filepath, n_samples, num_threads, n_critical, num_edge2scan);
    };


    for (auto &instance: real_instances) {
        if (instance.substr(instance.size() - 4) != ".txt") continue;
        auto instance_name = instance.substr(0, instance.size() - 4);
        std::cout << "Instance " << instance_name << ":\n";
        run_instance(real_dir + instance);
        std::cout << std::endl;
    }
}