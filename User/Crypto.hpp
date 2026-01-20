#pragma once
#include <string>

// Simple hash for password and username
inline std::string hashstr(const std::string& input) {
    std::hash<std::string> hasher;
    return std::to_string(hasher(input));
}