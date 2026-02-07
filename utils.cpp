
#include "utils.h"

NearlyBalancedParams::NearlyBalancedParams(const std::string &line) {
    std::regex re("\\s+");
    std::vector<int> v;
    for (auto it = std::sregex_token_iterator(line.begin(), line.end(), re, -1); it != std::sregex_token_iterator(); ++it)
        v.push_back(std::stoi(it->str()));
    if (v.size() != 7) throw std::runtime_error("Invalid string for nearly balanced params");
    n1_ = v[0];
    n2_ = v[1];
    m1_ = v[2];
    m2_ = v[3];
    btw_ = v[4];
    c1_ = v[5];
    c2_ = v[6];
}

template <typename T>
struct LineParser;

template <>
struct LineParser<int> {
    static int parse(const std::string& s) {
        return std::stoi(s);
    }
};

template <>
struct LineParser<double> {
    static double parse(const std::string& s) {
        return std::stod(s);
    }
};

template <>
struct LineParser<NearlyBalancedParams>{
    static NearlyBalancedParams parse(const std::string& s){
        return NearlyBalancedParams(s);
    }
};


template std::vector<int> read_params(const std::string& filepath);
template std::vector<double> read_params(const std::string& filepath);
template std::vector<NearlyBalancedParams> read_params(const std::string& filepath);

template <class T>
std::vector<T> read_params(const std::string& filepath){
    std::ifstream f(filepath);
    if (!f.is_open()) throw std::runtime_error("Can't open the file with p-factor");
    std::string line;
    LineParser<T> ps;
    std::vector<T> ans;
    while (std::getline(f, line)){
        if (line.empty()) break;
        ans.push_back(ps.parse(line));
    }

    return ans;
}

std::vector<std::string> get_dir_files(const std::string& dir_name){
    std::vector<std::string> instances;
    try{
        for (const auto& entry: std::filesystem::directory_iterator(dir_name)){
            instances.push_back(entry.path().filename().string());
        }
    } catch (const std::filesystem::filesystem_error& e) {
        auto err = std::string(e.what());
        throw std::runtime_error("Filesystem error: " + err);
    }
    return instances;
}
