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
                if(compare(el, needle)) goto BREAK_OUTER;
            }

            return false;

        BREAK_OUTER:
        }
        return true;
    }
}