#ifndef LINE_H
#define LINE_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <vector>
#include "split.hpp"

struct Line {
    Line(std::string line) {
        // split the line based on spaces
        std::vector<std::string> aux_elements = Helpers::split(line, ' ');
        std::vector<std::string> elements;

        // only add strings that are not empty
        // ignore everything after a semicolon since it is a comment
		for (auto it : aux_elements) {
			if(it[0] == ' ' || it.empty()) continue;
			if(it[0] == ';') break;
			elements.push_back(it);
		}

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
