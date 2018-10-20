#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <map>
#include "helpers.hpp"
#include "tables.hpp"

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

	bool is_number(std::string &s) {
		if(!std::isdigit(s[0]) && s[0] != '-' && s[0] != '+') {
			return false;
		}

		for(int i = 1; i < s.size(); i++) {
			if(!std::isdigit(s[i])) {
				return false;
			}
		}
		return true;
	}

	uint16_t get_value(std::string operand) {
		std::string aux;
		int pos = 0;
		uint8_t value = 0;
		int sign = 1;

		while (pos < operand.size()) {
			while (pos < operand.size() && operand[pos] != '+' && operand[pos] != '-') {
				aux += operand[pos++];
			}

			if(Helpers::is_number(aux)){
				value += sign *  std::stoi(aux);
			}
			else{
				value += sign * Tables::symbols.at(aux);
			}

			aux = "";
			if(pos < operand.size()) {
				sign = operand[pos++] == '+' ? 1 : -1;
			}
		}

		return value;
	}

} // end namespace Helpers

