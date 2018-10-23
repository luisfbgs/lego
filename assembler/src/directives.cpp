#include<vector>
#include<string>
#include "directives.hpp"
#include "line.hpp"
#include "twopass.hpp"
#include "tables.hpp"

void Directives::resolve_section(Line line) {
	if (line.operands.size() != 1) {
		TwoPass::error_list.push_back("Erro: Operando SECTION na linha " + 
				std::to_string(line.original_line));
		return;
	}

	bool error = true;
	if (line.operands[0] == "TEXT") {
		if (TwoPass::section == TwoPass::NONE) {
			TwoPass::section = TwoPass::TEXT;
			error = false;
		}
	}
	else if (line.operands[0] == "DATA") {
		if (TwoPass::section != TwoPass::NONE && TwoPass::section != TwoPass::DATA) {
			TwoPass::section = TwoPass::DATA;
			error = false;
		}
	}
	else if (line.operands[0] == "BSS") {
		if (TwoPass::section != TwoPass::NONE && TwoPass::section != TwoPass::BSS) {
			TwoPass::section = TwoPass::BSS;
			error = false;
		}
	}

	if (error) {
		TwoPass::error_list.push_back("Erro: Operando SECTION na linha " + 
				std::to_string(line.original_line) + " as secoes devem ser: " + 
				"TEXT, DATA ou BSS. Cada seção só pode ser definida uma vez e " +
				"a seção TEXT deve vir antes da outras");

	}
}

bool Directives::resolve_if (Line line) {
	std::vector<std::string> operands = line.operands;
	if (operands.size() != 1 || !Helpers::is_number(operands[0])) {
		TwoPass::error_list.push_back("Erro: Operando do IF na linha " +
				std::to_string(line.original_line));
	}
	else if (std::stoi(operands[0]) != 1) {
		return true;
	}
	return false;
}

void Directives::resolve_equ(Line line) {
	if (TwoPass::section != TwoPass::NONE) {
		TwoPass::error_list.push_back("Erro: EQU deve estar antes de qualquer seção, na linha " +
				std::to_string(line.original_line));
	}
	std::vector<std::string> operands = line.operands;
	if (operands.size() != 1) {
		TwoPass::error_list.push_back("Erro: Número de operandos no EQU na linha " +
				std::to_string(line.original_line));
	}
	else if (line.label.empty()) {
		TwoPass::error_list.push_back("Erro: EQU sem label na linha " +
				std::to_string(line.original_line));
	}
	else {
		Tables::equ[line.label] = line.operands[0];
	}
}

uint16_t Directives::resolve_space(Line line) {
	if (TwoPass::section != TwoPass::BSS) {
		TwoPass::error_list.push_back("Erro: CONST deve estar na seção TwoPass::BSS, na linha " +
				std::to_string(line.original_line));
		return 0;
	}
	std::vector<std::string> operands = line.operands;
	if (operands.size() > 1 || (operands.size() && !Helpers::is_number(operands[0]))) {
		TwoPass::error_list.push_back("Erro: Operando SPACE na linha" + 
				std::to_string(line.original_line));
		return 0;
	}
	else {
		uint16_t reserved_mem;
		if (operands.empty()) {
			reserved_mem = 1;
		}
		else {
			reserved_mem = stoi(operands[0]);
		}
		for (int i = 0; i < reserved_mem; i++) {
			Tables::can_read.insert(line.original_pos + i);
			Tables::can_write.insert(line.original_pos + i);
		}
	}
}

void Directives::resolve_const(Line line) {
	if (TwoPass::section != TwoPass::DATA) {
		TwoPass::error_list.push_back("Erro: CONST deve estar na seção DATA, na linha " +
				std::to_string(line.original_line));
		return;
	}
	std::vector<std::string> operands = line.operands;
	if (operands.size() != 1 || (operands.size() && !Helpers::is_number(operands[0]) && !Helpers::is_hex(operands[0]))) {
		TwoPass::error_list.push_back("Erro: Operando CONST na linha " + 
				std::to_string(line.original_line));
		return;
	}
	Tables::can_read.insert(line.original_pos);
}

void Directives::resolve_extern(Line line) {
	if (line.label.empty()) {
		TwoPass::error_list.push_back("Erro: EXTERN sem label, linha " +
				std::to_string(line.original_line));
		return;
	}
	if (!TwoPass::is_module) {
		TwoPass::error_list.push_back("Erro: EXTERN deve ser utilizada somente em módulos, linha " +
				std::to_string(line.original_line));
		return;
	}
	Tables::symbols[line.label] = 0;
	Tables::use.insert(line.label);
}

void Directives::resolve_public(Line line) {
	if (!TwoPass::is_module) {
		TwoPass::error_list.push_back("Erro: PUBLIC deve ser utilizada somente em módulos, linha " +
				std::to_string(line.original_line));
		return;
	}
	if (line.operands.size() != 1) {
		TwoPass::error_list.push_back("Erro: PUBLIC conter exatamente 1 operando, linha " +
				std::to_string(line.original_line));
		return;
	}
	Tables::definition.insert(line.operands[0]);
}

std::pair<uint16_t, bool> Directives::resolve(Line line) {
	std::string operation = line.operation;
	switch(Tables::directives.at(operation).id) {
		case 1: // IF
			return {0, resolve_if (line)};
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
		case 6: // PUBLIC
			resolve_public(line);
			break;
		case 7: // EXTERN
			resolve_extern(line);
			break;
		case 8: // BEGIN
			if (line.original_line != 1) {
				TwoPass::error_list.push_back("Erro: BEGIN deve ser utilizado na linha 1" +
						(" não na linha " + std::to_string(line.original_line)));
			}
			else {
				TwoPass::is_module = true;
			}
			break;
		case 9: // END
			if (!TwoPass::is_module) {
				TwoPass::error_list.push_back("Erro: END deve ser utilizado somente para " +
						("indicar o fim de um módulo, linha " + std::to_string(line.original_line)));
			}
			break;
	}
	return {0, false};
}
