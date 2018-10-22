#ifndef LINE_H
#define LINE_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <vector>
#include <algorithm>
#include "helpers.hpp"

struct Line {
    Line(std::string line, uint32_t original_line) : original_line(original_line) {
        // split the line based on spaces
        // only add strings that are not empty
        std::vector<std::string> elements = Helpers::split_invisible_semicolon(line);

        // if line does not have anything, return
		if (elements.empty()) return;

        // set label
        int pos = 0;
        if (elements[pos].back() == ':') {
            label = elements[pos];
			label.pop_back();
            pos += 1;
        }

        // check if line has anything after label
        // set operation
        if (pos >= elements.size()) return;
        operation = elements[pos];
        std::transform(operation.begin(), operation.end(), operation.begin(), ::tolower);
		pos += 1;

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
            	aux.clear();
			}
        }
        if (!aux.empty()) {
            operands.push_back(aux);
        }
    }

    std::string label;

    std::string operation;

    std::vector<std::string> operands;

	uint8_t type;

    uint32_t original_line;
};

#endif
