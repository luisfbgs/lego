#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <iterator>
#include "instructions.hpp"
#include "directives.hpp"
#include "line.hpp"

int resolve_directive(std::string s){
	return 1;
}

std::map<std::string, int> symbol_table;

bool first_pass(std::string source_file) {
	int position_count = 0;
	int line_count = 0;
	std::ifstream source;
	source.open(source_file);
	std::string raw_line;
	while (getline(source, raw_line)) {
		Line line(raw_line);

		if (!line.label.empty()) {
			std::string label = line.label;
			if (symbol_table.count(label)) {
				std::cout << "Erro: Rótulo " << label <<
					" redefinido na linha " << line_count << std::endl;
				return false;
			}
			else {
				symbol_table[label] = position_count;
			}
		}
		if (!line.operation.empty()) {
			std::string operation = line.operation;
			if (Tables::instruction_table.count(operation)) {
				position_count += Tables::instruction_table[operation].size;
			}
			else if (Tables::directive_table.count(operation)) {
				position_count += resolve_directive(operation);
			}
			else {
				std::cout << "Erro: Operação " << operation <<
					" não identificada na linha " << line_count << std::endl;
				return false;
			}
		}
		line_count++;
	}
	return true;
}
