#pragma once
#include <string>
#include <optional>
#include <nlohmann/json.hpp>
using namespace std;
template <typename T>
struct Res {
    bool ok;
    std::string msg;
    T data;

    static Res<T> success(T d, std::string m = "Success") {
        return {true, m, std::move(d)};
    }
    static Res<T> error(std::string m) {
        return {false, m, T()};
    }
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Res, ok, msg, data)
};

template <>
struct Res<void> {
    bool ok;
    std::string msg;

    static Res<void> success(std::string m = "Success") {
        return {true, m};
    }
    static Res<void> error(std::string m) {
        return {false, m};
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Res<void>, ok, msg)
};