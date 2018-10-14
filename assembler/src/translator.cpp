#include <string>
#include <iostream>
#include <vector>
#include "translator.hpp"
#include "instructions.hpp"
#include "directives.hpp"

Tables::Instruction instructions2;
Tables::Directive directives2;

bool is_number(std::string &s) {
	for (char c : s) {
		if (!std::isdigit(c)) {
			return false;
		}
	}
	return true;
}

uint8_t get_value(std::string operand, std::map<std::string, uint8_t> symbol_table) {
	std::string aux;
	int pos = 0;
	uint8_t value = 0;
	int sign = 1;

	while (pos < operand.size()) {
		while (pos < operand.size() && operand[pos] != '+' && operand[pos] != '-') {
			aux += operand[pos++];
		}
		
		if(is_number(aux)){
			value += sign *  std::stoi(aux);
		}
		else{
			value += sign * symbol_table[aux];
		}

		aux = "";
		if(pos < operand.size()) {
			sign = operand[pos++] == '+' ? 1 : -1;
		}
	}

	return value;
}

std::vector<uint8_t> to_machine_code(std::string operation, std::vector<std::string> operands, uint8_t type, std::map<std::string, uint8_t> symbol_table) {
	std::vector<uint8_t> machine_code;
	
	if (type == 1) {
		machine_code.push_back(instructions2.table[operation].op_code);
		for (std::string operand : operands) {
			machine_code.push_back(get_value(operand, symbol_table));
		}
	}
	else {
		machine_code.push_back(0);
	}
	return machine_code;
}
