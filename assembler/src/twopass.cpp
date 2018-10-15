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
#include "helpers.hpp"

int TwoPass::resolve_directive(std::string s) {
	return 1;
}

TwoPass::TwoPass(std::string source_file) {
	input_file = source_file + ".asm";
	pre_file   = source_file + ".pre";
	obj_file   = source_file + ".obj";

	source.open(input_file);
	pre.open(pre_file);
	obj.open(obj_file);
}

std::vector<Line> TwoPass::first_pass() {
	int position_count = 0;
	int line_count = 1;
	std::string raw_line;

	std::vector<Line> code;
	while (getline(source, raw_line)) {
		Line line(raw_line, line_count);

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
				position_count += Tables::instructions.at(operation).size;
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

void TwoPass::second_pass(std::vector<Line> code) {
	int position_count = 0;
	int line_count = 1;

	obj.open(obj_file);
	for (Line line : code) {
		std::string operation = line.operation;
		std::vector<std::string> operands = line.operands;
		
		if (line.type == 1) {
			if(operands.size() != Tables::instructions.at(operation).operands) {
				error_list.push_back("Erro: Operação " + operation + " requer " +
					std::to_string(Tables::instructions.at(operation).operands) +
					 " operadores mas foram providdos " + std::to_string(operands.size()) +
					  " operadores na linha " + std::to_string(line_count));
			}
			
			obj << Tables::instructions.at(operation).op_code;
		
			for (std::string operand : operands) {
				obj << Helpers::get_value(operand);
			}
		}
		else if (line.type == 2){
			if(operands.size() != Tables::directives.at(operation).operands) {
				error_list.push_back("Erro: Operação " + operation + " requer " +
					std::to_string(Tables::directives.at(operation).operands) +
					 " operadores mas foram providdos " + std::to_string(operands.size()) +
					  " operadores na linha " + std::to_string(line_count));
			}
		}

		line_count++;
	}
	obj.close();
}
