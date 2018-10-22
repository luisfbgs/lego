#include<vector>
#include<string>
#include "directives.hpp"
#include "line.hpp"
#include "twopass.hpp"
#include "tables.hpp"

void Directives::resolve_section(Line line) {
	if(line.operands.size() != 1) {
		TwoPass::error_list.push_back("Erro: Operando SECTION na linha " + 
				std::to_string(line.original_line));
		return;
	}

	bool error = true;
	if(line.operands[0] == "TEXT") {
		if(TwoPass::section == TwoPass::NONE) {
			TwoPass::section = TwoPass::TEXT;
			error = false;
		}
	}
	else if(line.operands[0] == "DATA") {
		if(TwoPass::section != TwoPass::NONE && TwoPass::section != TwoPass::DATA) {
			TwoPass::section = TwoPass::DATA;
			error = false;
		}
	}
	else if(line.operands[0] == "BSS") {
		if(TwoPass::section != TwoPass::NONE && TwoPass::section != TwoPass::BSS) {
			TwoPass::section = TwoPass::BSS;
			error = false;
		}
	}

	if(error) {
		TwoPass::error_list.push_back("Erro: Operando SECTION na linha " + 
				std::to_string(line.original_line) + " as secoes devem ser: " + 
				"TEXT, DATA ou BSS. Cada seção só pode ser definida uma vez e " +
				"a seção TEXT deve vir antes da outras");

	}
}

bool Directives::resolve_if(Line line) {
	std::vector<std::string> operands = line.operands;
	if(operands.size() != 1 || !Helpers::is_number(operands[0])) {
		TwoPass::error_list.push_back("Erro: Operando do IF na linha " +
				std::to_string(line.original_line));
	}
	else if(std::stoi(operands[0]) != 1) {
		return true;
	}
	return false;
}

void Directives::resolve_equ(Line line) {
	if(TwoPass::section != TwoPass::NONE) {
		TwoPass::error_list.push_back("Erro: EQU deve estar antes de qualquer seção, na linha " +
				std::to_string(line.original_line));
	}
	std::vector<std::string> operands = line.operands;
	if(operands.size() != 1) {
		TwoPass::error_list.push_back("Erro: Número de operandos no EQU na linha " +
				std::to_string(line.original_line));
	}
	else if(line.label.empty()) {
		TwoPass::error_list.push_back("Erro: EQU sem label na linha " +
				std::to_string(line.original_line));
	}
	else {
		Tables::equ[line.label] = line.operands[0];
	}
}

uint16_t Directives::resolve_space(Line line) {
	if(TwoPass::section != TwoPass::BSS) {
		TwoPass::error_list.push_back("Erro: CONST deve estar na seção TwoPass::BSS, na linha " +
				std::to_string(line.original_line));
		return 0;
	}
	std::vector<std::string> operands = line.operands;
	if(operands.size() > 1 || (operands.size() && !Helpers::is_number(operands[0]))) {
		TwoPass::error_list.push_back("Erro: Operando SPACE na linha" + 
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

void Directives::resolve_const(Line line) {
	if(TwoPass::section != TwoPass::DATA) {
		TwoPass::error_list.push_back("Erro: CONST deve estar na seção DATA, na linha " +
				std::to_string(line.original_line));
		return;
	}
	std::vector<std::string> operands = line.operands;
	if(operands.size() != 1 || (operands.size() && !Helpers::is_number(operands[0]) && !Helpers::is_hex(operands[0]))) {
		TwoPass::error_list.push_back("Erro: Operando CONST na linha " + 
				std::to_string(line.original_line));
	}
}


std::pair<uint16_t, bool> Directives::resolve(Line line) {
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
