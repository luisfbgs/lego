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
std::map<std::string, std::string> equ_table;

TwoPass::TwoPass(std::string source_file) {
	input_file = source_file + ".asm";
	pre_file   = source_file + ".pre";
	obj_file   = source_file + ".obj";

	source.open(input_file);
	pre.open(pre_file);
	obj.open(obj_file);
}

bool TwoPass::resolve_if(Line line) {
	std::vector<std::string> operands = line.operands;
	if(operands.size() != 1 || !Helpers::is_number(operands[0])) {
		error_list.push_back("Erro: Operando do IF na lina " +
			 std::to_string(line.original_line));
	}
	else if(std::stoi(operands[0]) != 1){
		return true;
	}
	return false;
}

void TwoPass::resolve_equ(Line line) {
	std::vector<std::string> operands = line.operands;
	if(operands.size() != 1){
		error_list.push_back("Erro: Número de operandos no EQU na linha " +
			 std::to_string(line.original_line));
	}
	else if(line.label.empty()) {
		error_list.push_back("Erro: EQU sem label na linha " +
			 std::to_string(line.original_line));
	}
	else {
		equ_table[line.label] = line.operands[0];
	}
}

uint16_t TwoPass::resolve_space(Line line) {
	std::vector<std::string> operands = line.operands;
	if(operands.size() > 1 || (operands.size() && !Helpers::is_number(operands[0]))) {
		error_list.push_back("Erro: Operando SPACE na linha" + 
			std::to_string(line.original_line));
		return 0;
	}
	else {
		if(operands.empty()) {
			return 1;
		}
		else {
			return std::stoi(operands[0]);
		}
	}
}

std::pair<uint16_t, bool> TwoPass::resolve_directive(Line line) {
	std::string operation = line.operation;
	switch(Tables::directives.at(operation).id) {
		case 1: // IF
			return {0, resolve_if(line)};
		case 2: // EQU
			resolve_equ(line);
			return {0, false};
			break;
		case 3: // SECTION
			break;
		case 4: // SPACE
			return {resolve_space(line), false};
			break;
	}
	return {0, false};
}


void TwoPass::store_label(Line line, int position_count) {
	std::string label = line.label;
	if (!label.empty()) {
		if (Tables::symbols.count(label)) {
			error_list.push_back("Erro: Simbolo " + label +
				" redefinido na linha " + std::to_string(line.original_line));
		}
		else {
			Tables::symbols[label] = position_count;
		}
	}
}

void TwoPass::replace_equ(std::vector<std::string> &operands) {
	for(std::string &operand : operands) {
		if(equ_table.count(operand)) {
			operand = equ_table[operand];
		}
	}
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
			line_count++;
			continue;
		}

		Line line(raw_line, line_count);

		TwoPass::store_label(line, position_count);

		TwoPass::replace_equ(line.operands);

		if (!line.operation.empty()) {
			std::string operation = line.operation;
			if (Tables::instructions.count(operation)) {
				position_count += Tables::instructions.at(operation).size;
				line.type = 1;
			}
			else if (Tables::directives.count(operation)) {
				int directive_size;
				std::tie(directive_size, ignore) = resolve_directive(line);
				position_count += directive_size;
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

	for(std::pair<std::string, std::string> equ : equ_table) {
		Tables::symbols.erase(equ.first);
	}

	return code;
}

void TwoPass::second_pass(std::vector<Line> code) {
	for (Line line : code) {
		std::string operation = line.operation;
		std::vector<std::string> operands = line.operands;

		if (line.type == 1) {
			if(operands.size() != Tables::instructions.at(operation).operands) {
				error_list.push_back("Erro: Operação " + operation + " requer " +
						std::to_string(Tables::instructions.at(operation).operands) +
						" operadores mas foram providdos " + std::to_string(operands.size()) +
						" operadores na linha " + std::to_string(line.original_line));
			}

			obj << std::to_string(Tables::instructions.at(operation).op_code) << " ";

			for (std::string operand : operands) {
				try { 
					obj << std::to_string(Helpers::get_value(operand)) << " ";
				}
				catch(const std::exception& error) {
					error_list.push_back("Erro: Símbolo não definido na linha " +
						std::to_string(line.original_line));
				}
			}
		}
		else if (line.type == 2){
			if(operands.size() != Tables::directives.at(operation).operands) {
				error_list.push_back("Erro: Operação " + operation + " requer " +
						std::to_string(Tables::directives.at(operation).operands) +
						" operadores mas foram providdos " + std::to_string(operands.size()) +
						" operadores na linha " + std::to_string(line.original_line));
			}
		}
	}
	obj << std::endl;
	obj.close();

	for(auto error : error_list) {
		std::cout << error << std::endl; 
	}
}
