#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

namespace utils::file {
    std::string read_contents(std::string path) {
        using std::ifstream, std::stringstream;

        ifstream f(path);
        stringstream buff;
        buff << f.rdbuf();
        return buff.str();
    }
}