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
#include "directives.hpp"

TwoPass::Sections TwoPass::section = TwoPass::NONE;
std::vector<std::string> TwoPass::error_list;
std::set<std::string> divs;
bool TwoPass::is_module = false;

TwoPass::TwoPass(std::string source_file) {
	input_file = source_file + ".asm";
	pre_file   = source_file + ".pre";
	obj_file   = source_file + ".obj";

	source.open(input_file);
	first_jump = last_jump = -1;
}

void TwoPass::check_mem_access(Line line) {
	if (line.operation == "load") {
		if (!Tables::can_read.count(Helpers::get_value(line.operands[0]))) {
			error_list.push_back("Erro: LOAD na linha " + std::to_string(line.original_line) +
					" não tem permissão para ler do endereço especificado");
			std::cout << Helpers::get_value(line.operands[0]) << std::endl;
		}
	}
	else if (line.operation == "input") {
		if (!Tables::can_write.count(Helpers::get_value(line.operands[0]))) {
			error_list.push_back("Erro: INPUT na linha " + std::to_string(line.original_line) +
					" não tem permissão para escrever no endereço especificado");
		}
	}
	else if (line.operation == "copy") {
		if (!Tables::can_write.count(Helpers::get_value(line.operands[1]))) {
			error_list.push_back("Erro: COPY na linha " + std::to_string(line.original_line) +
					" não tem permissão para escrever no endereço especificado");
		}
		if (!Tables::can_read.count(Helpers::get_value(line.operands[0]))) {
			error_list.push_back("Erro: COPY na linha " + std::to_string(line.original_line) +
					" não tem permissão para ler do endereço especificado");
		}
	}
	else if (line.operation == "jmpz") {
		uint16_t jmp_address = Helpers::get_value(line.operands[0]);
		if (jmp_address < first_jump || jmp_address > last_jump) {
			error_list.push_back("Erro: JMPZ na linha " + std::to_string(line.original_line) +
					" não tem permissão para pular para o endereço especificado");
			std::cout << line.operands[0] << std::endl;
			std::cout << "F " << int(first_jump) << " " << int(last_jump) << std::endl;
		}
	}
}

void TwoPass::write_directive(Line line) {
	std::string operation = line.operation;
	switch (Tables::directives.at(operation).id) {
		case 4: // SPACE
			if (line.operands.empty()) {
				obj_code.push_back(0);
			}
			else {
				for (int i = 0; i < std::stoi(line.operands[0]); i++) {
					obj_code.push_back(0);
				}
			}
			break;
		case 5: // CONST
			if (Helpers::is_number(line.operands[0])) {
				obj_code.push_back(std::stoi(line.operands[0]));
			}
			else {
				obj_code.push_back(strtol(line.operands[0].c_str(), NULL, 16));
			}

			if (obj_code.back() == 0 && divs.count(line.label)) {
				error_list.push_back("Erro: CONST 0 usado em divisao com label " + line.label);				
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
	for (std::string &operand : operands) {
		std::string new_operand;
		for (std::string symbol : Helpers::split_invisible_semicolon(operand)) {
			if (Tables::equ.count(symbol)) {
				new_operand += Tables::equ[symbol];
			}
			else {
				new_operand += symbol;
			}
		}
		operand = new_operand;
	}
}

std::vector<Line> TwoPass::first_pass() {
	int position_count = 0;
	int line_count = 1;
	std::string raw_line;

	std::vector<Line> code;
	bool ignore = false; // true if the next line must be ignored, false otherwise
	while (getline(source, raw_line)) {
		if (ignore) {
			ignore = false;
			line_count++;
			continue;
		}

		Line line(raw_line, line_count, position_count);

		if (line.additional_labels != "") {
			error_list.push_back("Erro: Dois ou mais rótulos na linha " + std::to_string(line.original_line));
		}

		while (line.operation.empty()) {
			if (!getline(source, raw_line)) {
				break;
			}
			line_count++;
			Line line2(raw_line, line_count, position_count);
			if (!line2.label.empty()) {
				TwoPass::store_label(line, position_count);
				line = line2;
				continue;
			}

			line2.label = line.label;
			line = line2;
		}

		
		TwoPass::store_label(line, position_count);

		TwoPass::replace_equ(line.operands);

		if (!line.operation.empty()) {
			std::string operation = line.operation;
			if (Tables::instructions.count(operation)) {
				if (section != TEXT) {
					error_list.push_back("Erro: Instruções devem estar na seção TEXT, na linha " +
							std::to_string(line.original_line));
				}
				position_count += Tables::instructions.at(operation).size;
				line.type = 1;
				code.push_back(line);
			}
			else if (Tables::directives.count(operation)) {
				uint16_t directive_size;
				std::tie(directive_size, ignore) = Directives::resolve(line);
				position_count += directive_size;

				if (section == TEXT && first_jump == -1) {
					first_jump = position_count;
				}

				line.type = 2;
				if (Tables::directives.at(line.operation).id > 2) {
					code.push_back(line);
				}
			}
			else {
				error_list.push_back("Erro: Operação " + operation +
						" não identificada na linha " + std::to_string(line_count));
			}
		}
		else {
			line.type = 0;
			code.push_back(line);
		}

		if (section == TEXT) {
			last_jump = position_count;
		}

		line_count++;
	}

	if (section == NONE) {
		error_list.push_back("Erro: Seção TEXT faltante");
	}

	if (is_module && code.back().operation != "end") {
		error_list.push_back("Erro: Módulos devem conter a diretiva END na última linha");
	}

	for (std::string public_label : Tables::definition) {
		if (!Tables::symbols.count(public_label)) {
			error_list.push_back("Erro: Label publica " + public_label +
					" não foi definida");
		}
		public_labels.push_back({public_label, Tables::symbols[public_label]});
	}

	if (error_list.empty()) {	
		pre.open(pre_file);

		for (Line line : code) {
			if (!line.label.empty()) {
				pre << line.label << ": ";
			}
			if (!line.operation.empty()) {
				pre << line.operation;
			}
			for (int i = 0; i < line.operands.size(); i++) {
				if (i) {
					pre << ",";
				}
				pre << " " << line.operands[i];
			}
			pre << std::endl;
		}

		pre.close();
	}
	for (std::pair<std::string, std::string> equ : Tables::equ) {
		Tables::symbols.erase(equ.first);
	}

	return code;
}

void TwoPass::second_pass(std::vector<Line> code) {
	for (Line line : code) {
		std::string operation = line.operation;
		std::vector<std::string> operands = line.operands;

		if (line.type == 1) {
			if (operands.size() != Tables::instructions.at(operation).operands) {
				error_list.push_back("Erro: Operação " + operation + " requer " +
						std::to_string(Tables::instructions.at(operation).operands) +
						" operadores mas foram providdos " + std::to_string(operands.size()) +
						" operadores na linha " + std::to_string(line.original_line));
				continue;
			}

			if (operation == "div") {
				divs.insert(operands[0]);
			}
			
			bool valid_operands = true;;
			
			obj_code.push_back(Tables::instructions.at(operation).op_code);

			for (std::string operand : operands) {
				try { 
					for (std::string symbol : Helpers::split_invisible_semicolon(operand)) {
						if (Tables::use.count(symbol)) {
							extern_use.push_back({symbol, obj_code.size()});
						}
						if (Tables::symbols.count(symbol)) {
							relative_addresses.push_back(obj_code.size());
						}
					}
					obj_code.push_back(Helpers::get_value(operand));
				}
				catch (const std::exception& error) {
					error_list.push_back("Erro: Símbolo não definido na linha " +
							std::to_string(line.original_line));
					valid_operands = false;
				}
			}

			TwoPass::check_mem_access(line);
		}
		else if (line.type == 2) {
			write_directive(line);
		}
	}

	if (error_list.empty()) {
		obj.open(obj_file);

		if (is_module) {
			obj << "TABLE USE" << std::endl;
			for (std::pair<std::string, uint16_t> use : extern_use) {
				obj << use.first << " " << use.second << std::endl;
			}
			obj << std::endl;

			obj << "TABLE DEFINITION" << std::endl;
			for (std::pair<std::string, uint16_t> public_label : public_labels) {
				obj << public_label.first << " " << public_label.second << std::endl;
			}
			obj << std::endl;

			obj << "RELATIVE" << std::endl;
			for (uint16_t relative_address : relative_addresses) {
				obj << relative_address << " ";
			}
			obj << std::endl;
			obj << std::endl;

			obj << "CODE" << std::endl;
		}

		for (uint16_t machine_code : obj_code) {
			obj << machine_code << " ";
		}
		obj << std::endl;
		obj.close();
	}

	for (auto error : error_list) {
		std::cout << error << std::endl; 
	}
}
