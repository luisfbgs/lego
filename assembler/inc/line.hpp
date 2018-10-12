#ifndef LINE_H
#define LINE_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <vector>
#include <algorithm>
#include "split.hpp"

struct Line {
    Line(std::string line) {
        // split the line based on spaces
        // only add strings that are not empty
        std::vector<std::string> elements = Helpers::split_invisible_semicolon(line);

        // if line does not have anything, return
		if (elements.empty()) return;

        // set label
        int pos = 0;
        if (elements[pos].back() == ':') {
            label = elements[pos];
            pos += 1;
        }

        // check if line has anything after label
        // set operation
        if (pos >= elements.size()) return;
        operation = elements[pos];
        std::transform(operation.begin(), operation.end(), operation.begin(), ::tolower);

        // iterate over remaining strings adding them to operands
        // add all substrings to a single string until a colon is found
        // or the line ends
        std::string aux;
        while (pos < elements.size()) {
            aux += elements[pos];
            pos += 1;
            if (aux.back() == ',') {
                aux.pop_back();
                operands.push_back(aux);
            }
        }
        if (!aux.empty()) {
            operands.push_back(aux);
        }
    }

    std::string label;

    std::string operation;

    std::vector<std::string> operands;
};

#endif
