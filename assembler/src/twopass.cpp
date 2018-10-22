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

TwoPass::TwoPass(std::string source_file) {
	input_file = source_file + ".asm";
	pre_file   = source_file + ".pre";
	obj_file   = source_file + ".obj";

	source.open(input_file);
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
		if(Tables::equ.count(operand)) {
			operand = Tables::equ[operand];
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
				std::tie(directive_size, ignore) = Directives::resolve(line);
				position_count += directive_size;

				line.type = 2;
				if(Tables::directives.at(line.operation).id > 2) {
					code.push_back(line);
				}
			}
			else {
				error_list.push_back("Erro: Operação " + operation +
						" não identificada na linha " + std::to_string(line_count));
			}
		}

		line_count++;
	}

	pre.open(pre_file);

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
	for(std::pair<std::string, std::string> equ : Tables::equ) {
		Tables::symbols.erase(equ.first);
	}

	return code;
}

void TwoPass::second_pass(std::vector<Line> code) {
	obj.open(obj_file);

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
