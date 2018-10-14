#ifndef HELPERS_H
#define HELPERS_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <string>
#include <vector>

namespace Helpers{

template<typename Out>
void split(const std::string &s, char delim, Out result);

std::vector<std::string> split(const std::string &s, char delim);

std::vector<std::string> split_invisible_semicolon(const std::string &s);

} // end namespace Helpers

#endif
