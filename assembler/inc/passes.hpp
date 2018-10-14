#ifndef PASSES_H
#define PASSES_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <vector>
#include "line.hpp"

std::vector<Line> first_pass(std::string source_file);

std::vector<uint8_t> second_pass(std::vector<Line>);

#endif
