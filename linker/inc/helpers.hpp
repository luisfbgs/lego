#ifndef HELPERS_H
#define HELPERS_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <string>
#include <vector>
#include <map>

namespace Helpers{

	template<typename Out>
		void split(const std::string &s, char delim, Out result);

	std::vector<std::string> split(const std::string &s, char delim);

	std::vector<std::string> split_invisible_semicolon(const std::string &s);

	uint16_t get_value(std::string operand);

	bool is_number(std::string &s);

	bool replace(std::string& str, const std::string& from, const std::string& to);

} // end namespace Helpers

#endif
