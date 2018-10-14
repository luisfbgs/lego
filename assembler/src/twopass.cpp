
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <iterator>
#include "twopass.hpp"
#include "tables.hpp"

int TwoPass::resolve_directive(std::string s) {
	return 1;
}

TwoPass::TwoPass(std::string source_file) {
	input_file = source_file + ".asm";
	pre_file   = source_file + ".pre";
	obj_file   = source_file + ".obj";

	source.open(input_file);
}

std::vector<Line> TwoPass::first_pass() {
	int position_count = 0;
	int line_count = 1;
	std::string raw_line;

	std::vector<Line> code;
	while (getline(source, raw_line)) {
		Line line(raw_line);

		if (!line.label.empty()) {
			std::string label = line.label;
			if (Tables::symbols.count(label)) {
				error_list.push_back("Erro: Rótulo " + label +
					" redefinido na linha " + std::to_string(line_count));
			}
			else {
				Tables::symbols[label] = position_count;
			}
		}
		if (!line.operation.empty()) {
			std::string operation = line.operation;
			if (Tables::instructions.count(operation)) {
				position_count += Tables::instructions[operation].size;
				line.type = 1;
			}
			else if (Tables::directives.count(operation)) {
				position_count += resolve_directive(operation);
				line.type = 2;
			}
			else {
				error_list.push_back("Erro: Operação " + operation +
					" não identificada na linha " + std::to_string(line_count));
			}

			code.push_back(line);
		}
		line_count++;
	}

	return code;
}

std::vector<uint8_t> TwoPass::second_pass(std::vector<Line> code) {
	int position_count = 0;
	int line_count = 1;

	std::vector<uint8_t> out;
	for (Line line : code) {
		
		std::string operation = line.operation;
		std::vector<std::string> operands = line.operands;

		if (line.type == 1) {
			if(operands.size() != Tables::instructions[operation].operands) {
				std::cout << "Erro: Operação " << operation << " requer " <<
					Tables::instructions[operation].operands << " operadores mas foram providdos " <<
					operands.size() << " operadores na linha " << line_count << std::endl;
				exit(0);
			}
		}
		else if (line.type == 2){
			if (operands.size() != Tables::directives[operation].operands) {
				std::cout << "Erro: Operação " << operation << " requer " <<
					Tables::instructions[operation].operands << " operadores mas foram providos " <<
					operands.size() << " operadores na linha " << line_count << std::endl;
				exit(0);
			}
		}
		line_count++;
	}
	return out;
}
