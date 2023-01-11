#pragma once

#include <vector>
#include <string.h>

namespace utils::list {
    using std::vector;

    template<typename T>
    inline bool compare(T a, T b) {
        return a == b;
    }

    template<>
    inline bool compare<const char*>(const char* a, const char* b) {
        return strcmp(a, b) == 0;
    }

    template<typename T>
    bool contains_all(vector<T> hay, vector<T> needles) {
        for(auto& needle : needles) {
            bool has_found = false;

            for(auto& el : hay) {
                has_found |= compare(el, needle);
            }

            if(!has_found) return false;
        }
        return true;
    }
}