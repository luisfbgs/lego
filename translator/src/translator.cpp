#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "translator.hpp"
#include "line.hpp"

Translator::Translator(std::string source_file) {
    input_file = source_file + ".asm";

    source.open(input_file);
}

std::vector<Line> Translator::pre_process() {
    std::vector<Line> code;

    std::string raw_line;
    while (getline(source, raw_line)) {
        Line line(raw_line);
        code.push_back(line);
    }

    return code;
}