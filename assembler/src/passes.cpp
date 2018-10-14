#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <iterator>
#include "passes.hpp"
#include "instructions.hpp"
#include "directives.hpp"

int resolve_directive(std::string s){
	return 1;
}

std::map<std::string, uint8_t> symbol_table;

Tables::Instruction instructions;
Tables::Directive directives;

std::vector<Line> first_pass(std::string source_file) {
	int position_count = 0;
	int line_count = 1;
	std::ifstream source;
	source.open(source_file);
	std::string raw_line;

	std::vector<Line> code;
	while (getline(source, raw_line)) {
		Line line(raw_line);

		if (!line.label.empty()) {
			std::string label = line.label;
			if (symbol_table.count(label)) {
				std::cout << "Erro: Rótulo " << label <<
					" redefinido na linha " << line_count << std::endl;
				exit(0);
			}
			else {
				symbol_table[label] = position_count;
			}
		}
		if (!line.operation.empty()) {
			std::string operation = line.operation;
			if (instructions.table.count(operation)) {
				position_count += instructions.table[operation].size;
				line.type = 1;
			}
			else if (directives.table.count(operation)) {
				position_count += resolve_directive(operation);
				line.type = 2;
			}
			else {
				std::cout << "Erro: Operação " << operation <<
					" não identificada na linha " << line_count << std::endl;
				exit(0);
			}

			code.push_back(line);
		}
		line_count++;
	}

	return code;
}

std::vector<uint8_t> second_pass(std::vector<Line> code) {
    int position_count = 0;
    int line_count = 1;

	std::vector<uint8_t> out;
    for (Line line : code) {
		
		std::string operation = line.operation;
		std::vector<std::string> operands = line.operands;

		if(line.type == 1){
			if(operands.size() != instructions.table[operation].operands){
				std::cout << "Erro: Operação " << operation << " requer " <<
					 instructions.table[operation].operands << " operadores mas foram providdos " <<
					 operands.size() << " operadores na linha " << line_count << std::endl;
				exit(0);
			}
		}
		else if(line.type == 2){
			if(operands.size() != directives.table[operation].operands){
				std::cout << "Erro: Operação " << operation << " requer " <<
					 instructions.table[operation].operands << " operadores mas foram providdos " <<
					 operands.size() << " operadores na linha " << line_count << std::endl;
				exit(0);
			}
		}
		line_count++;
    }
	return out;
}
