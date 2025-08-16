#include <vector>
#include <array>
#include <unordered_set>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cstdint>
#include <string_view>


consteval std::uint64_t compileTimehashString(std::string_view sv) {
    uint64_t hash = 1469598103934665603ULL;
    for(char c : sv) {
        hash = hash ^ static_cast<unsigned char>(c);
        hash = hash * 1099511628211ULL;
    }
    return hash;
}

std::uint64_t runTimeHashString(std::string_view sv) {
    uint64_t hash = 1469598103934665603ULL;
    for(char c : sv) {
        hash = hash ^ static_cast<unsigned char>(c);
        hash = hash * 1099511628211ULL;
    }
    return hash;
}



int main() {

    return 0;
}