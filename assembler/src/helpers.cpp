#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include "helpers.hpp"

namespace Helpers{

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

std::vector<std::string> split_invisible_semicolon(const std::string &s) {
    std::vector<std::string> elems;
    std::string aux;
    for (char c : s) {
        if (isspace(c) && !aux.empty()) {
            elems.push_back(aux);
            aux = "";
        }
        else if (c == ';') {
            break;
        }
        else if (!isspace(c)) {
            aux += c;
        }
    }
    if (!aux.empty()) elems.push_back(aux);
    return elems;
}

} // end namespace Helpers

