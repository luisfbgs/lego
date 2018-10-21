#include <iostream>
#include <string>
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
	else if(std::stoi(operands[0]) != 1) {
		return true;
	}
	return false;
}

void TwoPass::resolve_equ(Line line) {
	if(section != NONE) {
		error_list.push_back("Erro: EQU deve estar antes de qualquer seção, na linha " +
			 std::to_string(line.original_line));
	}
	std::vector<std::string> operands = line.operands;
	if(operands.size() != 1) {
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
	if(section != BSS) {
		error_list.push_back("Erro: CONST deve estar na seção BSS, na linha " +
			 std::to_string(line.original_line));
		return 0;
	}
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

void TwoPass::resolve_const(Line line) {
	if(section != DATA) {
		error_list.push_back("Erro: CONST deve estar na seção DATA, na linha " +
			 std::to_string(line.original_line));
		return;
	}
	std::vector<std::string> operands = line.operands;
	if(operands.size() != 1 || (operands.size() && !Helpers::is_number(operands[0]) && !Helpers::is_hex(operands[0]))) {
		error_list.push_back("Erro: Operando CONST na linha " + 
			std::to_string(line.original_line));
	}
}

void TwoPass::resolve_section(Line line) {
	if(line.operands.size() != 1) {
		error_list.push_back("Erro: Operando SECTION na linha " + 
			std::to_string(line.original_line));
		return;
	}

	bool error = false;
	if(line.operands[0] == "TEXT") {
		if(section == NONE) {
			section = TEXT;
		}
		else {
			error = true;
		}
	}
	else if(line.operands[0] == "DATA") {
		if(section == NONE || section == DATA) {
			error = true;
		}
		else {
			section = DATA;
		}
	}
	else if(line.operands[0] == "BSS") {
		if(section == NONE || section == BSS) {
			error = true;
		}
		else {
			section = BSS;
		}
	}
	else {
		error = true;
	}

	if(error) {
		error_list.push_back("Erro: Operando SECTION na linha " + 
			std::to_string(line.original_line) + " as secoes devem ser: " + 
			"TEXT, DATA ou BSS. Cada seção só pode ser definida uma vez e " +
			"a seção TEXT deve vir antes da outras");

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
		case 3: // SECTION
			resolve_section(line);
			return {0, false};
		case 4: // SPACE
			return {resolve_space(line), false};
		case 5: // CONST
			resolve_const(line);
			return {1, false};
	}
	return {0, false};
}

void TwoPass::write_directive(Line line) {
	std::string operation = line.operation;
	switch(Tables::directives.at(operation).id) {
		case 4: // SPACE
			if(line.operands.empty()) {
				obj << "0 ";
			}
			else {
				for(int i = 0; i < std::stoi(line.operands[0]); i++) {
					obj << "0 ";
				}
			}
			break;
		case 5: // CONST
			if(Helpers::is_number(line.operands[0])) {
				obj << std::stoi(line.operands[0]) << " ";
			}
			else {
				obj << strtol(line.operands[0].c_str(), NULL, 16) << " ";
			}
			break;
	}
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
				if(section != TEXT) {
				error_list.push_back("Erro: Instruções devem estar na seção TEXT, na linha " +
					 std::to_string(line.original_line));
				}
				position_count += Tables::instructions.at(operation).size;
				line.type = 1;
				code.push_back(line);
			}
			else if (Tables::directives.count(operation)) {
				int directive_size;
				std::tie(directive_size, ignore) = resolve_directive(line);
				position_count += directive_size;
				line.type = 2;
				if(Tables::directives.at(line.operation).id > 2)
					code.push_back(line);
			}
			else {
				error_list.push_back("Erro: Operação " + operation +
					" não identificada na linha " + std::to_string(line_count));
			}
		}

		line_count++;
	}

	for(Line line : code) {
		if(!line.label.empty()) {
			pre << line.label << ": ";
		}
		if(!line.operation.empty()) {
			pre << line.operation;
		}
		for(int i = 0; i < line.operands.size(); i++) {
			if(i) {
				pre << ",";
			}
			pre << " " << line.operands[i];
		}
		pre << std::endl;
	}

	pre.close();
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

			if(section != TEXT) {
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
		else if (line.type == 2) {
			write_directive(line);
		}
	}
	obj << std::endl;
	obj.close();

	for(auto error : error_list) {
		std::cout << error << std::endl; 
	}
}
