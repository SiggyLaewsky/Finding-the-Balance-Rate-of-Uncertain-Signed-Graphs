

#include <numeric>
#include "stat.h"
#include <boost/math/distributions/normal.hpp>

double empirical_variance(const std::vector<double>& res){
    double mean = std::accumulate(res.begin(), res.end(), 0.0) / ((double) res.size());
    double var = 0.0;
    for (auto it: res) var += (it - mean) * (it - mean);
    return var / ((double) res.size());
}

double empirical_variance(const std::vector<std::vector<double>>& res){
    double prod_mean = 1.0;
    double prod_sq_mean = 1.0;

    for (const auto& rj : res) {
        double mean = std::accumulate(rj.begin(), rj.end(), 0.0) / ((double) rj.size());

        double sq_mean = std::accumulate(
                rj.begin(), rj.end(), 0.0,
                [](double s, double v) { return s + v * v; }
        ) / ((double) rj.size());

        prod_mean    *= mean;
        prod_sq_mean *= sq_mean;
    }

    return prod_sq_mean - prod_mean * prod_mean;
}

prefix_CI get_hoeffding_CI(const std::vector<double>& r, double delta){
    prefix_CI ans;
    ans.mean_.resize(r.size());
    ans.lb_.resize(r.size());
    ans.ub_.resize(r.size());
    double sum = 0.0;
    for (auto pos = 0; pos < r.size(); ++pos){
        sum += r[pos];
        double val = sum / (1.0 + (double) pos);
        double eps = std::sqrt(std::log(2.0 / delta) / (2.0 * ((double) pos + 1.0)));
        ans.mean_[pos] = val;
        ans.ub_[pos] = std::min(1.0, val + eps);
        ans.lb_[pos] = std::max(0.0, val - eps);
    }
    ans.delta_ = delta;
    ans.descr_ = "Hoeffding prefix_CI, $\\delta = " + std::to_string(delta) + "$";
    return ans;
}

prefix_CI get_log_delta_CI(const std::vector<std::vector<double>>& r, double delta){
    struct RunningStats {
        size_t n = 0;
        double mean = 0.0;
        double M2 = 0.0;

        void update(double x) {
            ++n;
            double delta = x - mean;
            mean += delta / ((double) n);
            double delta2 = x - mean;
            M2 += delta * delta2;
        }

        [[nodiscard]] double variance() const {
            return (n > 1) ? M2 / (-1.0 + (double) n) : 0.0;
        }
    };

    if (r.empty()) throw std::logic_error("empty sample set");

    const size_t m = r.size();
    const size_t n = r[0].size();

    prefix_CI ans;
    ans.delta_ = delta;
    ans.mean_.resize(n);
    ans.lb_.resize(n);
    ans.ub_.resize(n);

    std::vector<RunningStats> stats(m);

    boost::math::normal norm(0.0, 1.0);
    double z = boost::math::quantile(norm, 1.0 - delta / 2.0);

    for (size_t k = 1; k <= n; ++k) {
        for (size_t j = 0; j < m; ++j)
            stats[j].update(r[j][k - 1]);

        if (k < 1) continue;

        double log_theta_hat = 0.0;
        double var_log_theta = 0.0;

        for (size_t j = 0; j < m; ++j) {
            double mu = std::max(stats[j].mean, 1e-12);
            double var = stats[j].variance();

            log_theta_hat += std::log(mu);
            var_log_theta += var / (mu * mu);
        }

        var_log_theta /= ((double) k);

        double hw = z * std::sqrt(var_log_theta);

        ans.mean_[k - 1] = std::exp(log_theta_hat);
        ans.lb_[k - 1] = std::exp(log_theta_hat - hw);
        ans.ub_[k - 1] = std::min(1.0, std::exp(log_theta_hat + hw));
    }

    return ans;
}