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

// Stores the values of EQUs labels
std::map<std::string, std::string> equs;

std::pair<uint16_t, int> TwoPass::resolve_directive(std::string operation, std::vector<std::string> operands) {
	switch(Tables::directives.at(operation).id) {
		case 1: // IF
			if(operands.size() != 1 || !Helpers::is_number(operands[0])) {
				error_list.push_back("Erro: Operando do IF");
			}
			else if(std::stoi(operands[0]) != 1){
				return {1, 1};
			}
			return {0, 1};
			break;
		case 2: // EQU
			if(operands.size() != 1){
				error_list.push_back("Erro: Número de operandos no EQU");
			}
			return {0, 2};
			break;
		case 3: // SECTION
			break;
		case 4: // SPACE
			if(operands.size() > 1 || (operands.size() && !Helpers::is_number(operands[0]))) {
				error_list.push_back("Erro: Operando SPACE");
			}
			else {
				if(operands.empty()) {
					return {1, 0};
				}
				else {
					return {std::stoi(operands[0]), 0};
				}

			}
			break;
	}
	return {0, 0};
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
	bool ignore = false; // true if the next line must be ignored, false otherwise
	while (getline(source, raw_line)) {
		if(ignore) {
			ignore = false;
			continue;
		}

		Line line(raw_line, line_count);

		if (!line.label.empty()) {
			std::string label = line.label;
			if (Tables::symbols.count(label)) {
				error_list.push_back("Erro: Simbolo " + label +
					" redefinido na linha " + std::to_string(line_count));
			}
			else {
				Tables::symbols[label] = position_count;
			}
		}

		for(std::string &operand : line.operands) {
			if(equs.count(operand)) {
				operand = equs[operand];
			}
		}

		if (!line.operation.empty()) {
			std::string operation = line.operation;
			if (Tables::instructions.count(operation)) {
				position_count += Tables::instructions.at(operation).size;
				line.type = 1;
			}
			else if (Tables::directives.count(operation)) {
				std::pair<uint16_t, int> result_directive = resolve_directive(operation, line.operands);
				// Check is the directive is IF
				if(result_directive.second == 1) {
					if(result_directive.first) {
						ignore = true;
					}			
				}
				// Check if the directive is EQU
				else if(result_directive.second == 2) {
					if(line.label.empty()) {
						error_list.push_back("Erro: EQU sem label");
					}
					else {
						equs[line.label] = line.operands[0];
					}
				}
				else {
					position_count += result_directive.first;
				}
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

			obj << std::to_string(Tables::instructions.at(operation).op_code) << " ";
		
			for (std::string operand : operands) {
				try { 
					obj << std::to_string(Helpers::get_value(operand)) << " ";
				}
				catch(const std::exception& error) {
					error_list.push_back("Erro: Símbolo não definido na linha " + std::to_string(line_count));
				}
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
	obj << std::endl;
	obj.close();

	for(auto error : error_list) {
		std::cout << error << std::endl; 
	}
}
